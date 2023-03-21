#ifndef rxmpfr_h_included
#define rxmpfr_h_included
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright Enrico Sorichetti 2023 - 2023
  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file BOOST_LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)
*/

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#if !defined( PREFIXED )
#define PREFIXED(_n)    rxmpfr ## _n
#endif

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#if !defined( _STR )
#define _STR(_S_)  _STR_(_S_)
#define _STR_(_S_) #_S_
#endif

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#if !defined( _MIN )
#define _MIN(_X_,_Y_)   ((_X_) < (_Y_) ? (_X_) : (_Y_) )
#endif

#if !defined( _MAX )
#define _MAX(_X_,_Y_)   ((_X_) > (_Y_) ? (_X_) : (_Y_) )
#endif

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#ifndef getArrayElement
#define getArrayElement( _X_, _A_, _I_ ) \
do { \
  RexxObjectPtr p ; \
  p = context->ArrayAt( (_A_), (_I_) ) ; \
  (_X_) = strtoull( context->ObjectToStringValue(p), NULL, BASE ) ; \
} while ( 0 )
#endif

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
//  #define fix_infinity( _X_, _L_ )

#ifndef fix_infinity
#define fix_infinity( _X_, _L_ ) \
do { \
  char iNeg[] = "-infinity" ; \
  char iPos[] = "+infinity" ; \
  if ( strcasecmp( (_X_),  "inf" ) == 0 ) \
    strncpy( (_X_), iPos, (_L_)  ) ;\
  else if ( strcasecmp( (_X_), "+inf" ) == 0 ) \
    strncpy( (_X_), iPos, (_L_)  ) ;\
  else if ( strcasecmp( (_X_), "-inf" ) == 0) \
    strncpy( (_X_), iNeg, (_L_)  ) ;\
} while ( 0 )
#endif

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#define CONSTANT( _N_, _C_ ) \
RexxRoutine0( RexxObjectPtr, _N_ ) { \
  int RC; int l; char f[16]; char b[BUFSZ]; \
  l = sprintf( f, "%s%lu%s", "%.", default_digits, "R*G" ) ; \
  l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, (_C_) ); \
  return ( context->NewString( b, l ) ); \
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#define CONVERT( _N_, _U_ ) \
RexxRoutine1( RexxObjectPtr, _N_, CSTRING, X ) { \
  int RC; int l; char f[16]; char b[BUFSZ]; \
  mpfr_t r, x ; \
  mpfr_inits2( default_precision, r, x, (mpfr_ptr) 0) ; \
  RC = mpfr_set_str( x, X, BASE, MPFR_RNDN ); \
  if ( RC != 0 ) { \
    context->InvalidRoutine(); \
    return context->NullString(); } \
  RC = mpfr_mul( r, _U_, x, MPFR_RNDN ) ; \
  l = sprintf( f, "%s%lu%s", "%.", default_digits, "R*G" ) ; \
  l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, r ); \
  mpfr_clears( r, x, (mpfr_ptr) 0); \
  mpfr_free_cache(); \
  return ( context->NewString( b, l ) ); \
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#define MATH( _N_, _F_ ) \
RexxRoutine1( RexxObjectPtr, _N_, CSTRING, X ) { \
  int RC; int l; char f[16]; char b[BUFSZ]; \
  mpfr_t r, x ; \
  mpfr_inits2( default_precision, r, x, (mpfr_ptr) 0) ; \
  RC = mpfr_set_str( x, X, BASE, MPFR_RNDN );  \
  if ( RC != 0 ) { \
    context->InvalidRoutine(); \
    return context->NullString(); } \
  RC = _F_( r, x, MPFR_RNDN ) ; \
  l = sprintf( f, "%s%lu%s", "%.", default_digits, "R*G" ) ; \
  l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, r ); \
  mpfr_clears( r, x, (mpfr_ptr) 0); \
  mpfr_free_cache(); \
  return ( context->NewString( b, l ) ); \
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#define MATH2(_N_, _F_) \
RexxRoutine2( RexxObjectPtr, _N_, CSTRING, X, CSTRING, Y ) { \
  int RC; int l; char f[16]; char b[BUFSZ]; \
  mpfr_t r, x, y ; \
  mpfr_inits2( default_precision, r, x, y, (mpfr_ptr) 0) ; \
  RC = mpfr_set_str( x, X, BASE, MPFR_RNDN );  \
  if ( RC != 0 ) { \
    context->InvalidRoutine(); \
    return context->NullString(); } \
  RC = mpfr_set_str( y, Y, BASE, MPFR_RNDN );  \
  if ( RC != 0 ) { \
    context->InvalidRoutine(); \
    return context->NullString(); } \
  RC = _F_(r, x, y, MPFR_RNDN ) ; \
  l = sprintf( f, "%s%lu%s", "%.", default_digits, "R*G" ) ; \
  l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, r ); \
  mpfr_clears( r, x, y, (mpfr_ptr) 0); \
  mpfr_free_cache(); \
  return ( context->NewString( b, l ) ); \
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#define TRIG( _N_, _F_ ) \
RexxRoutine3( RexxObjectPtr, _N_, CSTRING, X, \
  OPTIONAL_uint32_t, Prec, OPTIONAL_CSTRING, Units ) { \
  int RC; int l; char f[16]; char b[BUFSZ]; \
    mpfr_t r, x ; \
    mpfr_inits2( default_precision, r, x, (mpfr_ptr) 0) ; \
    RC = mpfr_set_str( x, X, BASE, MPFR_RNDN ) ;  \
    if ( RC != 0 ) { \
      context->InvalidRoutine(); \
      return context->NullString(); } \
    if ( Units == NULL || *Units == 'D' || *Units == 'd' ) { \
      RC = mpfr_mul( x, x, M_radians , MPFR_RNDN ) ; } \
    RC = _F_( r, x, MPFR_RNDN ) ; \
    adjust( r, r ) ; \
    l = sprintf( f, "%s%lu%s", "%.", default_digits, "R*G" ) ; \
    l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, r ) ; \
    fix_infinity( b, BUFHI ) ;\
    l = strlen( b ) ; \
    mpfr_clears( r, x, (mpfr_ptr) 0) ; \
    mpfr_free_cache() ; \
    return (RexxObjectPtr)context->NewString( b, l ) ; \
    }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#define ARCT( _N_, _F_ ) \
RexxRoutine3( RexxObjectPtr, _N_, CSTRING, X, \
  OPTIONAL_uint32_t, Prec, OPTIONAL_CSTRING, Units ) { \
  int RC; int l; char f[16]; char b[BUFSZ]; ; \
  mpfr_t r, x ; \
  mpfr_inits2( default_precision, r, x, (mpfr_ptr) 0) ; \
  RC = mpfr_set_str( x, X, BASE, MPFR_RNDN ) ;  \
  if ( RC != 0 ) { \
    context->InvalidRoutine() ; \
    return context->NullString() ; } \
  RC = _F_( r, x, MPFR_RNDN ) ; \
  if ( Units == NULL || *Units == 'D' || *Units == 'd' ) { \
    RC = mpfr_mul( r, r, M_degrees , MPFR_RNDN ) ; } \
  l = sprintf( f, "%s%lu%s", "%.", default_digits, "R*G" ) ; \
  l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, r ) ; \
  mpfr_clears( r, x, (mpfr_ptr) 0) ; \
  mpfr_free_cache() ; \
  return (RexxObjectPtr)context->NewString( b, l ) ; \
}

#endif  //  rxmpfr_h_included
