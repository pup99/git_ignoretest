// The user must define CLA_C in the project linker settings if using the
// CLA C compiler
// Project Properties -> C2000 Linker -> Advanced Options -> Command File
// Preprocessing -> --define
#ifdef CLA_C
// Define a size for the CLA scratchpad area that will be used
// by the CLA compiler for local symbols and temps
// Also force references to the special symbols that mark the
// scratchpad are.
CLA_SCRATCHPAD_SIZE = 0x100;
--undef_sym=__cla_scratchpad_end
--undef_sym=__cla_scratchpad_start
#endif //CLA_C
MEMORY
{
PAGE 0 :  /* Program Memory */
          /* Memory (RAM/FLASH) blocks can be moved to PAGE1 for data allocation */
          /* BEGIN is used for the "boot to Flash" bootloader mode   */

   BEGIN	       : origin = 0x080000,   length = 0x000002

   RESET           : origin = 0x3FFFC0,   length = 0x000002

#ifdef CPU1
   RAM_RUN         : origin = 0x013000,   length = 0x004000
#endif

#ifdef CPU2
   RAM_RUN         : origin = 0x00A800,   length = 0x001800
#endif

   /* Flash sectors */
   FLASHA          : origin = 0x080002,   length = 0x001FFE	/* on-chip Flash */
   FLASHB          : origin = 0x082000,   length = 0x002000	/* on-chip Flash */
   FLASHC          : origin = 0x084000,   length = 0x002000	/* on-chip Flash */
   FLASHD          : origin = 0x086000,   length = 0x002000	/* on-chip Flash */
   FLASHE          : origin = 0x088000,   length = 0x008000	/* on-chip Flash */
   FLASHF          : origin = 0x090000,   length = 0x008000	/* on-chip Flash */
   FLASHG          : origin = 0x098000,   length = 0x008000	/* on-chip Flash */
   FLASHH          : origin = 0x0A0000,   length = 0x008000	/* on-chip Flash */
   FLASHI          : origin = 0x0A8000,   length = 0x008000	/* on-chip Flash */
   FLASHJ          : origin = 0x0B0000,   length = 0x008000	/* on-chip Flash */
   FLASHK          : origin = 0x0B8000,   length = 0x002000	/* on-chip Flash */
   FLASHL          : origin = 0x0BA000,   length = 0x002000	/* on-chip Flash */
   FLASHM          : origin = 0x0BC000,   length = 0x002000	/* on-chip Flash */
   FLASHN          : origin = 0x0BE000,   length = 0x002000	/* on-chip Flash */

PAGE 1 : /* Data Memory */
         /* Memory (RAM/FLASH) blocks can be moved to PAGE0 for program allocation */

   BOOT_RSVD       : origin = 0x000002,   length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   RAM_STACK       : origin = 0x000122,   length = 0x0006DE
   RAM_ELSE        : origin = 0x008000,   length = 0x000800

#ifdef CPU1
   RAM_EBSS        : origin = 0x008800,   length = 0x00A800
#endif

#ifdef CPU2
   RAM_EBSS        : origin = 0x008800,   length = 0x002000
#endif

   RAM_CPU1_GS     : origin = 0x017000,   length = 0x003000
   RAM_CPU2_GS     : origin = 0x01A000,   length = 0x002000

//   EMIF1_CS0n      : origin = 0x80000000, length = 0x10000000
   EMIF1_CS2n      : origin = 0x00100000, length = 0x00200000
   EMIF1_CS3n      : origin = 0x00300000, length = 0x00080000
   EMIF1_CS4n      : origin = 0x00380000, length = 0x00060000
   EMIF2_CS0n      : origin = 0x90000000, length = 0x10000000
   EMIF2_CS2n      : origin = 0x00002000, length = 0x00001000

   CLA1_MSGRAMLOW  : origin = 0x001480,   length = 0x000080
   CLA1_MSGRAMHIGH : origin = 0x001500,   length = 0x000080

   CPU2TOCPU1RAM   : origin = 0x03F800,   length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00,   length = 0x000400

   UPP_MSGRAMTX	   : origin = 0x006C00,   length = 0x000100
   UPP_MSGRAMRX    : origin = 0x006E00,   length = 0x000100
   SDRAM 		   : origin = 0x80000000, length = 0x10000000 /* 512Kx16 SDRAM */

}

SECTIONS
{
   /* Allocate program areas: */
   .cinit          : >  FLASHB,     			            PAGE = 0, ALIGN(4)
   .pinit          : >  FLASHB,     			            PAGE = 0, ALIGN(4)
   .text           : >> FLASHB | FLASHC | FLASHD | FLASHE,  PAGE = 0, ALIGN(4)
   codestart       : >  BEGIN,       			            PAGE = 0, ALIGN(4)

   /* Allocate uninitalized data sections: */
   .stack          : >  RAM_STACK,        			        PAGE = 1
   .ebss           : >  RAM_EBSS,						    PAGE = 1
   Filter_RegsFile : >  RAM_EBSS,	   			            PAGE = 1

   .esysmem        : >  RAM_ELSE,			                PAGE = 1
   .cio            : >  RAM_ELSE,    			            PAGE = 1
   
   /* Initalized sections go in Flash */
   .econst         : >> FLASHF | FLASHG | FLASHH,     	    PAGE = 0, ALIGN(4)
   .switch         : >  FLASHB,				                PAGE = 0, ALIGN(4)

   .reset          : >  RESET,     			                PAGE = 0, TYPE = DSECT /* not used, */
   .farbss         : >  SDRAM,                              PAGE = 1

   SHARE_GSRAM_CPU1		 : > RAM_CPU1_GS,					PAGE = 1
   SHARE_GSRAM_CPU2		 : > RAM_CPU2_GS,				    PAGE = 1

#ifdef CPU1
#ifdef __TI_COMPILER_VERSION__
	#if __TI_COMPILER_VERSION__ >= 15009000
	.TI.ramfunc : {}     LOAD = FLASHD | FLASHE,
						 RUN = RAM_RUN,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
			             PAGE = 0, ALIGN(4)
	#else
   	ramfuncs           : LOAD = FLASHD  | FLASHE,
						 RUN = RAM_RUN,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(4)    
    #endif
#endif
#endif //END #ifdef CPU1

#ifdef CPU2
#ifdef __TI_COMPILER_VERSION__
	#if __TI_COMPILER_VERSION__ >= 15009000
	.TI.ramfunc : {}     LOAD = FLASHD | FLASHE,
		                 RUN = RAM_RUN,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
			             PAGE = 0, ALIGN(4)
	#else
   	ramfuncs           : LOAD = FLASHD  | FLASHE,
                         RUN = RAM_RUN,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(4)
    #endif
#endif
#endif //END #ifdef CPU2

   Cla1ToCpuMsgRAM : >  CLA1_MSGRAMLOW,   		            PAGE = 1
   CpuToCla1MsgRAM : >  CLA1_MSGRAMHIGH,  		            PAGE = 1

#ifdef CLA_C
   /* CLA C compiler sections */
   //
   // Must be allocated to memory the CLA has write access to
   //
   CLAscratch      :
                    { *.obj(CLAscratch)
                    . += CLA_SCRATCHPAD_SIZE;
                    *.obj(CLAscratch_end) } >  RAMLS5,      PAGE = 1

   .scratchpad     : >  RAMLS5,       	                    PAGE = 1
   .bss_cla	       : >  RAMLS5,              			    PAGE = 1
   .const_cla	   :  LOAD = FLASHD,
                      RUN = RAMLS5,
                      RUN_START(_Cla1ConstRunStart),
                      LOAD_START(_Cla1ConstLoadStart),
                      LOAD_SIZE(_Cla1ConstLoadSize),
                      PAGE = 1
#endif //CLA_C


#ifdef CPU1 
   /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        GETBUFFER   :    TYPE = DSECT
        GETWRITEIDX :    TYPE = DSECT
        PUTREADIDX  :    TYPE = DSECT
    }
#endif

#ifdef CPU2 
   /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }
#endif
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
