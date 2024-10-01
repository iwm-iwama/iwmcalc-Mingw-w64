//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2024 iwm-iwama"
#define   IWM_FILENAME        "iwmcalc"
#define   IWM_UPDATE          "20241001"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"
#include <mruby.h>
#include <mruby/compile.h>

INT
main()
{
	// lib_iwmutil2 初期化
	imain_begin();

	///CLI_VarList();

	// -h | --help
	if(! $ARGC || iCLI_getOptMatch(0, L"-h", L"--help"))
	{
		P1(
			"\n"
			IESC_OPT22
			"\033[3G" "+  -  *  /  %%  **  Sqrt(NUM)\n"
			"\033[3G" "Pi  Rad  Sin(NUM)  Cos(NUM)  Tan(NUM)\n"
			"\033[3G" "E  Log(base, NUM)\n"
			"\033[3G" "(mruby)Math module\n"
			"\n"
			"\033[3G"  IESC_LBL1 "(例１)"
			"\033[10G" IESC_STR1 IWM_FILENAME " 9 / 3 * (1+2)" IESC_OPT21 " => 9\n"
			"\033[3G"  IESC_LBL1 "(例２)"
			"\033[10G" IESC_STR1 IWM_FILENAME " i1=3**2; i2=4; i1/i2" IESC_OPT21 " => 2.25\n"
			"\033[3G"  IESC_LBL1 "(例３)"
			"\033[10G" IESC_STR1 IWM_FILENAME " Sin(30 * Rad)" IESC_OPT21 " => 0.5\n"
			"\033[3G"  IESC_LBL1 "(例４)"
			"\033[10G" IESC_STR1 IWM_FILENAME " Log(2, 16)" IESC_OPT21 " => 4\n"
			"\033[3G"  IESC_LBL1 "(例５)"
			"\033[10G" IESC_STR1 IWM_FILENAME " Math.log2(16)" IESC_OPT21 " => 4\n"
			"\n"
			IESC_RESET
		);
		imain_end();
	}

	WS *wp1 = iwas_join($ARGV, L"");
	WS *wp2 = iws_cats(
		4,
		// 1..3
		L"Args = '",
		wp1,
		L"';",
		// 4
		L"\
		def Log(d1, d2);\
		return 0.0 if d1 <= 0.0 || d2 <= 0.0;\
		Math::log(d2) / Math::log(d1);\
		end;\
		begin;\
		puts(\
		eval(\
		Args\
		.gsub('/', '.to_f/')\
		.gsub('Sqrt(',  'Math::sqrt(')\
		.gsub('Sin(',   'Math::sin(')\
		.gsub('Cos(',   'Math::cos(')\
		.gsub('Tan(',   'Math::tan(')\
		.gsub('Rad',    'Math::PI/180')\
		.gsub('Pi',     'Math::PI')\
		.gsub('E',      'Math::E')\
		).to_s.gsub(/\\.0+$/, '')\
		);\
		rescue => e;\
		puts \"\033[91m[Err] #{e.to_s.strip}\033[0m\";\
		rescue Exception => e;\
		puts \"\033[91m[Err] #{e.to_s.strip}\033[0m\";\
		end\
		"
	);

		MS *mp1 = W2M(wp2);
			/*
				program load functions

				Please note! Currently due to interactions with the GC calling these functions will
				leak one RProc object per function call.
				To prevent this save the current memory arena before calling and restore the arena
				right after, like so

					int ai = mrb_gc_arena_save(mrb);
					mrb_value status = mrb_load_string(mrb, buffer);
					mrb_gc_arena_restore(mrb, ai);

				Also, when called from a C function defined as a method, the current stack is destroyed.
				If processing continues after this function, the objects obtained from the arguments
				must be protected as needed before this function.
			*/
			mrb_state *mrb = mrb_open();
				INT i1 = mrb_gc_arena_save(mrb);
					mrb_load_string(mrb, mp1);
				mrb_gc_arena_restore(mrb, i1);
			mrb_close(mrb);
		ifree(mp1);

	ifree(wp2);
	ifree(wp1);

	///idebug_map(); ifree_all(); idebug_map();

	// 最終処理
	imain_end();
}
