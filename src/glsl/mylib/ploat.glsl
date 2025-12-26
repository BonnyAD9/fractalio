#ifndef PLOAT_INCLUDED
#define PLOAT_INCLUDED

// Value may be changed in preprocessing.
#define PRECISION 4

#if PRECISION == 8

#define ploat double
#define pvec2 dvec2

#else

#define ploat float
#define pvec2 vec2

#endif

#endif // PLOAT_INCLUDED