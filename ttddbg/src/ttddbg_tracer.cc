#include "ttddbg_tracer.hh"
#include "ttddbg_callconv.hh"

#include <Windows.h>

#include <kernwin.hpp>
#include <pro.h>
#include <funcs.hpp>
#include <name.hpp>
#include <typeinf.hpp>

#define BITNESS_x86 1
#define BITNESS_x64 2


namespace ttddbg {
	void callCallback(unsigned __int64 callback_value, TTD::GuestAddress addr_func, TTD::GuestAddress addr_ret, struct TTD::TTD_Replay_IThreadView* thread_view) {
		if (addr_ret != 0) {
			// Case 1: callback because we executed a "call" instruction
			//	addr_func = argument to "call" (address)
			//	addr_ret = where we will "ret" to (the instruction right after "call")

			if (FunctionTracer::getInstance()->isEATraced(addr_func)) {
				func_t* func = get_func(addr_func);
				TTD::Position* position = thread_view->IThreadView->GetPosition(thread_view);

				FunctionInvocation ev{ 0 };
				ev.func = func;
				ev.pos = *position;
				FunctionTracer::getInstance()->recordCall(ev);
			}

		}
		else {
			// Case 2: callback because we executed a "ret" instruction
			//	addr_func = address to the next instruction (after the "ret")
			// NOTE: the Program Counter is still at the "ret" instruction!

			TTD::Position* position = thread_view->IThreadView->GetPosition(thread_view);
			TTD::GuestAddress pc = thread_view->IThreadView->GetProgramCounter(thread_view);

			func_t* func = get_func(pc);
			if (func == nullptr) {
				return;
			}

			if (!FunctionTracer::getInstance()->isEATraced(func->start_ea)) {
				return;
			}
			
			FunctionTracer::getInstance()->recordRet(func->start_ea, *position);
		}
	}

	FunctionTracer* FunctionTracer::c_instance = nullptr;

	FunctionTracer::FunctionTracer() {

	}

	void FunctionTracer::destroy() {
		if (FunctionTracer::c_instance != nullptr) {
			//FunctionTracer::getInstance()->m_cursor->SetCallReturnCallback(NULL, 0);
			delete FunctionTracer::c_instance;
			FunctionTracer::c_instance = NULL;
		}
	}

	void FunctionTracer::setNewTraceCallback(std::function<void(func_t*)> f) {
		m_cbNewTrace = f;
	}

	void FunctionTracer::setNewEventCallback(std::function<void(FunctionInvocation)> f) {
		m_cbNewEvent = f;
	}

	FunctionTracer* FunctionTracer::getInstance() {
		if (FunctionTracer::c_instance == nullptr) {
			FunctionTracer::c_instance = new FunctionTracer();
		}

		return FunctionTracer::c_instance;
	}

	void FunctionTracer::setCursor(std::shared_ptr<TTD::Cursor> cursor) {
		m_cursor = cursor;
		m_cursor->SetCallReturnCallback(&callCallback, 0);
	}

	void FunctionTracer::setEngine(TTD::ReplayEngine engine) {
		m_engine = engine;
	}

	void FunctionTracer::traceFunction(func_t *func) {
		if (isTraced(func))
			return;
		
		msg("[tracer] Tracing function at 0x%X\n", func->start_ea);
		m_traces.push_back(func->start_ea);

		if (m_cbNewTrace) {
			m_cbNewTrace(func);
		}
	}

	void FunctionTracer::removeTrace(size_t n) {
		m_traces.erase(m_traces.begin() + n);
	}
	
	void FunctionTracer::removeEvent(size_t n) {
		m_events.erase(m_events.begin() + n);
	}

	bool FunctionTracer::isTraced(func_t* func) {
		return isEATraced(func->start_ea);
	}

	bool FunctionTracer::isEATraced(ea_t start) {
		for (int i = 0; i < m_traces.size(); i++) {
			ea_t ea = m_traces.at(i);
			if (ea == start) {
				return true;
			}
		}
		return false;
	}

	void FunctionTracer::recordCall(FunctionInvocation ev) {		
		int bitness = get_func_bitness(ev.func);
		tinfo_t tinfo;
		get_tinfo(&tinfo, ev.func->start_ea);
		int nargs = tinfo.get_nargs();

		TTD::Cursor tmpCur = m_engine.NewCursor();
		tmpCur.SetPosition(&ev.pos);

		qstring mangled_name, func_name;
		get_func_name(&mangled_name, ev.func->start_ea);
		func_name = demangle_name(mangled_name.c_str(), 0);

		tinfo_t arg;
		qstring type_name;
		qstring value;
		
		// We will go through each argument of the function
		// and fetch it from memory. Then, it's rendered as a string
		// and added to the "ev.args" std::vector
		for (size_t i = 0; i < tinfo.get_nargs(); i++) {
			arg = tinfo.get_nth_arg(i);
			type_name.clear();
			arg.print(&type_name);
			value.clear();

			if (arg.is_int()) {
				if (bitness == BITNESS_x64) {
					value.sprnt("%d", x64_getIntArg(&tmpCur, tinfo, i));
				}
				else if (bitness == BITNESS_x86) {
					value.sprnt("%d", x86_getIntArg(&tmpCur, tinfo, i));
				}
			}
			else if (arg.is_ptr_or_array()) {
				unsigned long long ptr;
				if (bitness == BITNESS_x64) {
					ptr = x64_getIntArg(&tmpCur, tinfo, i);
				}
				else if (bitness == BITNESS_x86) {
					ptr = x86_getIntArg(&tmpCur, tinfo, i);
				}
				else {
					msg("Invalid bitness: %d\n", bitness);
					return;
				}
				
				tinfo_t pointed = arg.get_ptrarr_object();
				qstring pointed_type;
				pointed.print(&pointed_type);
				pointed_type.replace("const ", "");

				qstring tmp;
				if (pointed.is_char()) {
					tmp = readStringAt(&tmpCur, ptr);
					value.sprnt("\"%s\"", tmp.c_str());
				}
				else if (pointed_type == "wchar_t") {
					tmp = readWideStringAt(&tmpCur, ptr);
					value.sprnt("\"%s\"", tmp.c_str());
				}
				else {
					value.sprnt("0x%X", ptr);
				}
			}
			else if (arg.is_enum()) {
				enum_type_data_t enumDetails;
				bool ok = arg.get_enum_details(&enumDetails);
				if (!ok) {
					warning("Could not get enum details!");
				}
				else {
					int arg_value = 0;
					if (bitness == BITNESS_x64) {
						arg_value = x64_getIntArg(&tmpCur, tinfo, i);
					}
					else if (bitness == BITNESS_x86) {
						arg_value = x86_getIntArg(&tmpCur, tinfo, i);
					}

					for (size_t i = 0; i < enumDetails.size(); i++) {
						enum_member_t member = enumDetails.at(i);
						if (member.value == arg_value) {
							value = member.name;
							break;
						}
					}
				}
			}
				
			if (value.size() == 0){
				value = "?";
			}

			qstring tmp = value;
			value.sprnt("(%s)%s", type_name.c_str(), tmp.c_str());
			ev.args.push_back(value);
		}
			

		m_events.push_back(ev);
		sortEvents();

		if (m_cbNewEvent != nullptr) {
			m_cbNewEvent(ev);
		}
	}

	void FunctionTracer::recordRet(ea_t function_addr, TTD::Position pos)
	{	
		func_t* func = get_func(function_addr);
		int bitness = get_func_bitness(func);
		tinfo_t tinfo;
		get_tinfo(&tinfo, func->start_ea);

		TTD::Cursor tmpCur = m_engine.NewCursor();
		tmpCur.SetPosition(&pos);

		tinfo_t rettype = tinfo.get_rettype();
		
		qstring value;
		qstring type_name;

		rettype.print(&type_name);

		if (rettype.is_int()) {
			if (bitness == BITNESS_x64) {
				value.sprnt("%d", tmpCur.GetContextx86_64()->Rax);
			}
			else if (bitness == BITNESS_x86) {
				value.sprnt("%d", tmpCur.GetContextx86()->Eax);
			}
		}
		else if (rettype.is_ptr_or_array()) {
			unsigned long long ptr;
			if (bitness == BITNESS_x64) {
				ptr = tmpCur.GetContextx86_64()->Rax;
			}
			else if (bitness == BITNESS_x86) {
				ptr = tmpCur.GetContextx86()->Eax;
			}
			else {
				msg("Invalid bitness: %d\n", bitness);
				return;
			}

			tinfo_t pointed = rettype.get_ptrarr_object();
			qstring pointed_type;
			pointed.print(&pointed_type);
			pointed_type.replace("const ", "");

			qstring tmp;
			if (pointed.is_char()) {
				tmp = readStringAt(&tmpCur, ptr);
				value.sprnt("\"%s\"", tmp.c_str());
			}
			else if (pointed_type == "wchar_t") {
				tmp = readWideStringAt(&tmpCur, ptr);
				value.sprnt("\"%s\"", tmp.c_str());
			}
			else {
				value.sprnt("0x%X", ptr);
			}
		}
		else if (rettype.is_enum()) {
			enum_type_data_t enumDetails;
			bool ok = rettype.get_enum_details(&enumDetails);
			if (!ok) {
				warning("Could not get enum details!");
			}
			else {
				int arg_value = 0;
				if (bitness == BITNESS_x64) {
					arg_value = tmpCur.GetContextx86_64()->Rax;
				}
				else if (bitness == BITNESS_x86) {
					arg_value = tmpCur.GetContextx86()->Eax;
				}

				for (size_t i = 0; i < enumDetails.size(); i++) {
					enum_member_t member = enumDetails.at(i);
					if (member.value == arg_value) {
						value = member.name;
						break;
					}
				}
			}
		}

		if (value.size() == 0) {
			value = "?";
		}

		qstring tmp = value;
		value.sprnt("(%s)%s", type_name.c_str(), tmp.c_str());

		FunctionInvocation ev;
		ev.pos = pos;
		ev.func = func;
		ev.args = std::vector<qstring>();
		ev.is_return = true;

		ev.args.push_back(value);

		m_events.push_back(ev);
		sortEvents();

		if (m_cbNewEvent != nullptr) {
			m_cbNewEvent(ev);
		}
	}

	/*****************************************************************/

	size_t FunctionTracer::countTraced() {
		return m_traces.size();
	}

	func_t* FunctionTracer::funcAt(int i) {
		ea_t ea = m_traces.at(i);
		return get_func(ea);
	}

	/*****************************************************************/

	size_t FunctionTracer::countEvents() {
		return m_events.size();
	}

	FunctionInvocation FunctionTracer::eventAt(int i) {
		return m_events.at(i);
	}

	void FunctionTracer::sortEvents() {
		sort(m_events.begin(), m_events.end(), [](auto p1, auto p2) -> bool {
			return (p1.pos < p2.pos);
		});
	}

	void FunctionTracer::copyArgumentAddress(size_t nevent, size_t narg) {
		FunctionInvocation ev = eventAt(nevent);
		int bitness = get_func_bitness(ev.func);
		tinfo_t tinfo;
		get_tinfo(&tinfo, ev.func->start_ea);
		int nargs = tinfo.get_nargs();

		if (narg >= nargs) {
			warning("Function expects %d arguments, cannot get arg %d", nargs, narg);
			return;
		}

		TTD::Cursor tmpCur = m_engine.NewCursor();
		tmpCur.SetPosition(&ev.pos);

		func_type_data_t funcdata;
		tinfo.get_func_details(&funcdata);
		cm_t cc = funcdata.get_cc();

		qstring clip;
		tinfo_t arg = tinfo.get_nth_arg(narg);
		int copy_pointer_contents = ASKBTN_NO;
		if (arg.is_ptr_or_array()) {
			// In the case of a pointer: copy the address stored in the pointer
			copy_pointer_contents = ask_yn(ASKBTN_YES, "This argument is a pointer. Do you want to instead copy the address stored in the pointer?");

			if (copy_pointer_contents == ASKBTN_YES) {
				unsigned long long ptr = 0;
				if (bitness == BITNESS_x86) {
					ptr = x86_getIntArg(&tmpCur, tinfo, narg);
				}
				else if (bitness == BITNESS_x64) {
					ptr = x64_getIntArg(&tmpCur, tinfo, narg);
				}

				clip.sprnt("0x%X", ptr);
			}
		}

		if (copy_pointer_contents == ASKBTN_NO) {
			// In the case of a stack variable: copy the address of the variable on the stack
			// Of course, this doesn't work if the variable is not on the stack (__fastcall, x64)
			if (bitness == BITNESS_x86) {
				size_t firstStackArg = 0;
				if (cc == CM_CC_FASTCALL && narg < 2) {
					warning("This argument is passed by register. Cannot copy the address.");
					return;
				}
				else if (cc == CM_CC_FASTCALL) {
					firstStackArg = 2;
				}
				size_t espOffset = stackArgOffset(tinfo, narg, firstStackArg);

				DWORD32 esp = tmpCur.GetContextx86()->Esp;
				clip.sprnt("0x%X", esp + espOffset);
			}
			else if (bitness == BITNESS_x64) {
				if (narg < 4) {
					warning("This argument is passed by register. Cannot copy the address.");
					return;
				}

				size_t rspOffset = stackArgOffset(tinfo, narg, 4);

				DWORD64 rsp = tmpCur.GetContextx86_64()->Rsp;
				clip.sprnt("0x%X", rsp + rspOffset);
			}
		}

		setClipboardContent(clip);
	}

	void FunctionTracer::copyReturnValue(size_t nevent) 
	{
		FunctionInvocation ev = eventAt(nevent);
		int bitness = get_func_bitness(ev.func);
		TTD::Cursor tmpCur = m_engine.NewCursor();
		tmpCur.SetPosition(&ev.pos);

		unsigned long long ret = 0;
		if (bitness == BITNESS_x86) {
			ret = tmpCur.GetContextx86()->Eax;
		}
		else if (bitness == BITNESS_x64) {
			ret = tmpCur.GetContextx86_64()->Rax;
		}

		qstring out;
		out.sprnt("0x%X", ret);

		setClipboardContent(out);
	}

	void FunctionTracer::setClipboardContent(qstring s)
	{
		// Source: https://learn.microsoft.com/en-us/windows/win32/dataxchg/using-the-clipboard#copying-information-to-the-clipboard
		if (!OpenClipboard(NULL)) {
			warning("Could not open clipboard!");
			return;
		}
		EmptyClipboard();

		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE,
			(s.size() + 1) * sizeof(TCHAR));
		if (hglbCopy == NULL)
		{
			CloseClipboard();
			return;
		}

		LPTSTR  lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
		memcpy(lptstrCopy, s.c_str(),
			s.size() * sizeof(TCHAR));
		lptstrCopy[s.size()] = (TCHAR)0;    // null character 
		GlobalUnlock(hglbCopy);

		SetClipboardData(CF_TEXT, hglbCopy);

		CloseClipboard();
	}
}