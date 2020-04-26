bin_PROGRAMS = unidesc uniname unihist ExplicateUTF8 unireverse unifuzz
bin_SCRIPTS = utf8lookup
dist_man_MANS = unidesc.1 uniname.1 ExplicateUTF8.1 unihist.1 unireverse.1 unifuzz.1
unidesc_SOURCES = Get_UTF32_From_UTF8i.c unidesc.c Get_UTF32i.c Read_UMagic_Number.c unirange.c ExplicateBadUTF8.c binfmt.c
uniname_SOURCES = uniname.c Get_UTF32_From_UTF8i.c Get_UTF32i.c unames.c uranges.c unirange.c offsettbl.c NelsonTbl.c putu8.c ExplicateBadUTF8.c binfmt.c
unihist_SOURCES = unihist.c Get_UTF32_From_UTF8i.c  putu8.c ExplicateBadUTF8.c binfmt.c
ExplicateUTF8_SOURCES = ExplicateUTF8.c binfmt.c
unireverse_SOURCES = unireverse.c
unifuzz_SOURCES = unifuzz.c putu8.c unirange.c
noinst_HEADERS = nelson.h  offsettbl.h  unicode.h  uniftypes.h  unirange.h  utf8error.h
LOCALEDIR=/usr/local/share/locale
AM_CFLAGS = -DLOCALEDIR=\"${LOCALEDIR}\"
AUTOMAKE_OPTIONS = dist-zip dist-bzip2
EXTRA_DIST = genunames.awk TestData utf8lookup CREDITS










