/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright Enrico Sorichetti 2023 - 2023
  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file BOOST_LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)
*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oorexxapi.h"

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#include "oorexxapi.h"

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#include <mpfr.h>
#include "rxmpfr.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
#define BASE    10
#define BUFSZ   1024
#define BUFHI   BUFSZ - 8

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
static mpfr_exp_t       default_precision ;
static mpfr_exp_t       default_digits ;

static mpfr_t           default_overflow ;

static mpfr_t           M_pi;
static mpfr_t           M_pi_2 ;

static mpfr_t           M_degrees;
static mpfr_t           M_radians;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
static void
adjust( mpfr_t r, mpfr_t x )
{
  char *str = NULL ;
  mpfr_exp_t xpn, xpn2 ;
  mpfr_exp_t digits = default_digits - 4 ;
  int sign = ( mpfr_signbit(x) == 0 ? 1 : -1 ) ;
  //  get the decimal exponent
  str = mpfr_get_str( str, &xpn,   10, default_precision, x, MPFR_RNDN ) ;
  //  get the binary exponent
  str = mpfr_get_str( str, &xpn2,   2, default_precision, x, MPFR_RNDN ) ;
  if ( xpn >=  digits ) {
    mpfr_set_inf( r, sign ) ; }
  else
  if ( xpn <=  (-1*digits) ) {
    mpfr_set_zero( r, sign ) ; }
  else {
      mpfr_set( r, x, MPFR_RNDN ) ;
  }
  if ( str != NULL )
    mpfr_free_str( str ) ;
  return ;
}

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    initialisation
*/
static
void rxmpfrInit( RexxThreadContext *context )
{
  int RC; int l; char b[BUFSZ];

  //  set the default to the float128 precision
  default_precision = 113 ;
  mpfr_set_default_prec( default_precision ) ;

  //  compute the appropriate  ..._DECIMAL_DIG_
  default_digits = mpfr_get_str_ndigits(BASE, default_precision) ;

  //  the system provided constants
  mpfr_inits2( default_precision,
    M_pi, M_pi_2, M_degrees, M_radians,
    (mpfr_ptr) 0) ;

  //  pi and pi/2
  RC = mpfr_const_pi( M_pi, MPFR_RNDN ) ;
  RC = mpfr_div_ui( M_pi_2, M_pi, 2, MPFR_RNDN ) ;

  //  the degrees radians multipliers
  RC = mpfr_ui_div( M_degrees, 180, M_pi, MPFR_RNDN ) ;
  RC = mpfr_div_ui( M_radians, M_pi, 180, MPFR_RNDN ) ;

  return ;
}

static void rxmpfrTerm( RexxThreadContext *context )
{
  mpfr_clears(  M_pi, M_pi_2, M_degrees, M_radians,
    (mpfr_ptr) 0 ) ;
  mpfr_free_cache ();
  return ;
}

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ooRexx version check
*/


/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    mpfr version check
*/
RexxRoutine1( RexxObjectPtr, rxmpfrVersion ,
  OPTIONAL_CSTRING, Warn )
{
  //  size_t ooRexxVersion=getInterpreterVersion();

  int RC; int l; char b[BUFSZ];
  RexxArrayObject vers ;
  vers = context->NewArray(0);

  l = snprintf( b, BUFHI, "%s", MPFR_VERSION_STRING ) ;
  RC = context->ArrayAppendString(vers, b, l);
  l = snprintf( b, BUFHI, "%s", mpfr_get_version() );
  RC = context->ArrayAppendString(vers, b, l);


  if ( Warn != NULL )
  {
    fprintf (stderr, "Warning: checking build/runtime versions \n");
    if ( strcmp (mpfr_get_version (), MPFR_VERSION_STRING) )
      fprintf (stderr, "Warning: header and library do not match\n");
  }

  return( vers ) ;

}
;
/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    configuration
*/
RexxRoutine1( RexxObjectPtr, rxmpfrConfig ,
  OPTIONAL_RexxArrayObject, C )
{
  int RC; int l; char f[16]; char b[BUFSZ];;

  RexxArrayObject conf ;
  conf = context->NewArray(0);

  uint64_t precision, digits;

  // to skip the update if no args
  precision = default_precision ;
  digits = default_digits ;

  if ( argumentExists(1) )
  {
    if ( context->ArrayItems( C ) == 0 )
    {
      context->InvalidRoutine();
      return context->NullString();
    }

    getArrayElement( precision, C, 1 ) ;
    digits = mpfr_get_str_ndigits(BASE, precision ) ;
  }

  if ( precision != mpfr_get_default_prec() )
  {
    //  set the default to the requested precision
    default_precision = precision ;
    mpfr_set_default_prec( default_precision ) ;

    //  re-compute the appropriate  ..._DECIMAL_DIG_
    default_digits = mpfr_get_str_ndigits(BASE, default_precision) ;

    // clear and reinitialize the constants
    mpfr_clears(  M_pi, M_pi_2, M_degrees, M_radians,
      (mpfr_ptr) 0 ) ;
    mpfr_free_cache ();

    mpfr_inits2( default_precision,
      M_pi, M_pi_2, M_degrees, M_radians, (mpfr_ptr) 0) ;

    //  pi and pi/2
    RC = mpfr_const_pi( M_pi, MPFR_RNDN ) ;
    RC = mpfr_div_ui( M_pi_2, M_pi, 2, MPFR_RNDN ) ;

    //  the degrees radians multipliers
    RC = mpfr_ui_div( M_degrees, 180, M_pi, MPFR_RNDN ) ;
    RC = mpfr_div_ui( M_radians, M_pi, 180, MPFR_RNDN ) ;

  }
  else
  if ( digits != default_digits )
  {
    context->InvalidRoutine();
    return context->NullString();
  }

  //  return the current/changed settings

  l = snprintf( b, BUFHI, "%ld", default_precision ) ;
  RC = context->ArrayAppendString(conf, b, l);

  l = snprintf( b, BUFHI, "%ld", default_digits ) ;
  RC = context->ArrayAppendString(conf, b, l);

  return ( conf ) ;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    pre computed constants
*/
CONSTANT( PREFIXED(M_pi),       M_pi )
CONSTANT( PREFIXED(M_pi_2),     M_pi_2 )
CONSTANT( PREFIXED(M_degrees),  M_degrees )
CONSTANT( PREFIXED(M_radians),  M_radians )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    conversions
*/
CONVERT( PREFIXED(Degrees),   M_degrees )
CONVERT( PREFIXED(Radians),   M_radians )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    math one/one
*/
MATH( PREFIXED(Sqrt),   mpfr_sqrt )
MATH( PREFIXED(Cbrt),   mpfr_cbrt )
MATH( PREFIXED(Exp),    mpfr_exp )
MATH( PREFIXED(Expm1),  mpfr_expm1 )
MATH( PREFIXED(Exp2),   mpfr_exp2 )
MATH( PREFIXED(Log),    mpfr_log )
MATH( PREFIXED(Log1p),  mpfr_log1p )
MATH( PREFIXED(Log2),   mpfr_log2 )
MATH( PREFIXED(Log10),  mpfr_log10 )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    math two/one
*/
MATH2( PREFIXED(Pow),   mpfr_pow )
MATH2( PREFIXED(Hypot), mpfr_hypot )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    trig one/one
*/
TRIG( PREFIXED(Cos),  mpfr_cos )
TRIG( PREFIXED(Sin),  mpfr_sin )
TRIG( PREFIXED(Tan),  mpfr_tan )
TRIG( PREFIXED(Sec),  mpfr_sec )
TRIG( PREFIXED(Cot),  mpfr_cot )
TRIG( PREFIXED(Csc),  mpfr_csc )

#if 0
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    special sincos, one/two
*/
RexxRoutine3( RexxObjectPtr, rxmpfrSinCos, CSTRING, X,
  OPTIONAL_uint32_t, Prec, OPTIONAL_CSTRING, Units ) {
  int RC; int l; char f[16]; char b[BUFSZ];
  int lSin; char bSin[BUFSZ];
  int lCos; char bCos[BUFSZ];
  mpfr_t x, rSin, rCos ;
  mpfr_inits2( default_precision,
    x, rSin, rCos, (mpfr_ptr) 0) ;
  RC = mpfr_set_str( x, X, BASE, MPFR_RNDN ) ;
  if ( RC != 0 ) {
    context->InvalidRoutine();
    return context->NullString() ;
  }
  if ( Units == NULL || *Units == 'D' || *Units == 'd' ) {
      RC = mpfr_mul( x, x, M_radians , MPFR_RNDN ) ;
  }
  RC = mpfr_sin_cos( rSin, rCos, x, MPFR_RNDN ) ;
  l = sprintf( f, "%s%lu%s", "%.", default_digits , "R*G" ) ;
  lSin = mpfr_snprintf( bSin, BUFHI, f , MPFR_RNDN, rSin ) ;
  lSin = strlen( bSin ) ;
  lCos = mpfr_snprintf( bCos, BUFHI, f , MPFR_RNDN, rCos ) ;
  lCos = strlen( bCos ) ;
  return context->ArrayOfTwo(
    context->NewString( bSin, lSin ),
    context->NewString( bCos, lCos ) ) ;
}
#endif

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  sincos special
*/
RexxRoutine5( RexxObjectPtr, rxmpfrSinCos, CSTRING, X,
  OPTIONAL_CSTRING, S, OPTIONAL_CSTRING, C,
  OPTIONAL_uint32_t, Prec, OPTIONAL_CSTRING, Units )
{
  int RC; int l; char f[16]; char b[BUFSZ];
  mpfr_t x, s, c ;
  mpfr_inits2( default_precision,
    x, s, c, (mpfr_ptr) 0) ;

  RexxArrayObject r;
  r = context->NewArray(0) ;

  RC = mpfr_set_str( x, X, BASE, MPFR_RNDN ) ;
  if ( RC != 0 ) {
    context->InvalidRoutine();
    return context->NullString() ;
  }
  if ( Units == NULL || *Units == 'D' || *Units == 'd' ) {
      RC = mpfr_mul( x, x, M_radians , MPFR_RNDN ) ;
  }

  RC = mpfr_sin_cos( s, c, x, MPFR_RNDN ) ;

  l = sprintf( f, "%s%lu%s", "%.", default_digits , "R*G" ) ;

  l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, s ) ;
  RC = context->ArrayAppendString( r, b, l );
  if ( argumentExists(2) && argumentExists(3) )
    context->SetContextVariable( S, context->NewString( b, l) );

  l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, c ) ;
  RC = context->ArrayAppendString( r, b, l ) ;
  if ( argumentExists(2) && argumentExists(3) )
    context->SetContextVariable( C, context->NewString( b, l) );

  return (r) ;

}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    arct one/one
*/
ARCT( PREFIXED(Acos),   mpfr_acos )
ARCT( PREFIXED(Asin),   mpfr_asin )
ARCT( PREFIXED(Atan),   mpfr_atan )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    atan2 special, two/one
*/
RexxRoutine4( RexxObjectPtr, rxmpfrAtan2, CSTRING, X, CSTRING, Y,
  OPTIONAL_uint32_t, Prec, OPTIONAL_CSTRING, Units ) {
  int RC; int l; char f[16]; char b[BUFSZ]; ;
  mpfr_t r, x, y ;
  mpfr_inits2( default_precision, r, x, y, (mpfr_ptr) 0) ;
  RC = mpfr_set_str( x, X, BASE, MPFR_RNDN );
  if ( RC != 0 ) {
    context->InvalidRoutine();
    return context->NullString() ;
  }
  RC = mpfr_set_str( y, Y, BASE, MPFR_RNDN );
  if ( RC != 0 ) {
    context->InvalidRoutine();
    return context->NullString() ;
  }
  RC = mpfr_atan2( r, x, y, MPFR_RNDN ) ;
  if ( Units == NULL || *Units == 'D' || *Units == 'd' ) {
    RC = mpfr_mul( r, r, M_degrees , MPFR_RNDN ) ;
  }
  l = sprintf( f, "%s%lu%s", "%.", default_digits, "R*G" ) ;
  l = mpfr_snprintf( b, BUFHI, f , MPFR_RNDN, r ) ;
  mpfr_clears( r, x, y, (mpfr_ptr) 0) ;
  mpfr_free_cache() ;
  return (RexxObjectPtr)context->NewString( b, l ) ;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
RexxRoutineEntry rxmpfr_functions[] =
{
// long names
  REXX_TYPED_ROUTINE( PREFIXED(config),   PREFIXED(Config) ),

  REXX_TYPED_ROUTINE( PREFIXED(version),  PREFIXED(Version) ),

  REXX_TYPED_ROUTINE( PREFIXED(m_pi),     PREFIXED(M_pi) ),
  REXX_TYPED_ROUTINE( PREFIXED(m_degrees),PREFIXED(M_degrees) ),
  REXX_TYPED_ROUTINE( PREFIXED(m_radians),PREFIXED(M_radians) ),

  REXX_TYPED_ROUTINE( PREFIXED(degrees),  PREFIXED(Degrees) ),
  REXX_TYPED_ROUTINE( PREFIXED(radians),  PREFIXED(Radians) ),

  REXX_TYPED_ROUTINE( PREFIXED(sqrt),     PREFIXED(Sqrt) ),
  REXX_TYPED_ROUTINE( PREFIXED(cbrt),     PREFIXED(Cbrt) ),
  REXX_TYPED_ROUTINE( PREFIXED(pow),      PREFIXED(Pow) ),
  REXX_TYPED_ROUTINE( PREFIXED(hypot),    PREFIXED(Hypot) ),

  REXX_TYPED_ROUTINE( PREFIXED(exp),      PREFIXED(Exp) ),
  REXX_TYPED_ROUTINE( PREFIXED(expm1),    PREFIXED(Expm1) ),
  REXX_TYPED_ROUTINE( PREFIXED(exp2),     PREFIXED(Exp2) ),

  REXX_TYPED_ROUTINE( PREFIXED(log),      PREFIXED(Log) ),
  REXX_TYPED_ROUTINE( PREFIXED(log1p),    PREFIXED(Log1p) ),
  REXX_TYPED_ROUTINE( PREFIXED(log2),     PREFIXED(Log2) ),
  REXX_TYPED_ROUTINE( PREFIXED(log10),    PREFIXED(Log10) ),

  REXX_TYPED_ROUTINE( PREFIXED(cos),      PREFIXED(Cos) ),
  REXX_TYPED_ROUTINE( PREFIXED(sin),      PREFIXED(Sin) ),
  REXX_TYPED_ROUTINE( PREFIXED(tan),      PREFIXED(Tan) ),
  REXX_TYPED_ROUTINE( PREFIXED(sec),      PREFIXED(Sec) ),
  REXX_TYPED_ROUTINE( PREFIXED(cot),      PREFIXED(Cot) ),
  REXX_TYPED_ROUTINE( PREFIXED(csc),      PREFIXED(Csc) ),
  REXX_TYPED_ROUTINE( PREFIXED(cotan),    PREFIXED(Cot) ),
  REXX_TYPED_ROUTINE( PREFIXED(cosec),    PREFIXED(Csc) ),

  REXX_TYPED_ROUTINE( PREFIXED(sincos),   PREFIXED(SinCos) ),

  REXX_TYPED_ROUTINE( PREFIXED(acos),     PREFIXED(Acos) ),
  REXX_TYPED_ROUTINE( PREFIXED(asin),     PREFIXED(Asin) ),
  REXX_TYPED_ROUTINE( PREFIXED(atan),     PREFIXED(Atan) ),
  REXX_TYPED_ROUTINE( PREFIXED(arccos),   PREFIXED(Acos) ),
  REXX_TYPED_ROUTINE( PREFIXED(arcsin),   PREFIXED(Asin) ),
  REXX_TYPED_ROUTINE( PREFIXED(arctan),   PREFIXED(Atan) ),

  REXX_TYPED_ROUTINE( PREFIXED(atan2),    PREFIXED(Atan2) ),
  REXX_TYPED_ROUTINE( PREFIXED(arctan2),  PREFIXED(Atan2) ),

//  short names
  REXX_TYPED_ROUTINE( config,             PREFIXED(Config) ),

  REXX_TYPED_ROUTINE( version,            PREFIXED(Version) ),

  REXX_TYPED_ROUTINE( m_pi,               PREFIXED(M_pi) ),
  REXX_TYPED_ROUTINE( m_degrees,          PREFIXED(M_degrees) ),
  REXX_TYPED_ROUTINE( m_radians,          PREFIXED(M_radians) ),

  REXX_TYPED_ROUTINE( degrees,            PREFIXED(Degrees) ),
  REXX_TYPED_ROUTINE( radians,            PREFIXED(Radians) ),

  REXX_TYPED_ROUTINE( sqrt,               PREFIXED(Sqrt) ),
  REXX_TYPED_ROUTINE( cbrt,               PREFIXED(Cbrt) ),
  REXX_TYPED_ROUTINE( pow,                PREFIXED(Pow) ),
  REXX_TYPED_ROUTINE( hypot,              PREFIXED(Hypot) ),

  REXX_TYPED_ROUTINE( exp,                PREFIXED(Exp) ),
  REXX_TYPED_ROUTINE( expm1,              PREFIXED(Expm1) ),
  REXX_TYPED_ROUTINE( exp2,               PREFIXED(Exp2) ),

  REXX_TYPED_ROUTINE( log,                PREFIXED(Log) ),
  REXX_TYPED_ROUTINE( log1p,              PREFIXED(Log1p) ),
  REXX_TYPED_ROUTINE( log2,               PREFIXED(Log2) ),
  REXX_TYPED_ROUTINE( log10,              PREFIXED(Log10) ),

  REXX_TYPED_ROUTINE( cos,                PREFIXED(Cos) ),
  REXX_TYPED_ROUTINE( sin,                PREFIXED(Sin) ),
  REXX_TYPED_ROUTINE( tan,                PREFIXED(Tan) ),
  REXX_TYPED_ROUTINE( sec,                PREFIXED(Sec) ),
  REXX_TYPED_ROUTINE( cot,                PREFIXED(Cot) ),
  REXX_TYPED_ROUTINE( csc,                PREFIXED(Csc) ),
  REXX_TYPED_ROUTINE( cotan,              PREFIXED(Cot) ),
  REXX_TYPED_ROUTINE( cosec,              PREFIXED(Csc) ),

  REXX_TYPED_ROUTINE( sincos,             PREFIXED(SinCos) ),

  REXX_TYPED_ROUTINE( acos,               PREFIXED(Acos) ),
  REXX_TYPED_ROUTINE( asin,               PREFIXED(Asin) ),
  REXX_TYPED_ROUTINE( atan,               PREFIXED(Atan) ),
  REXX_TYPED_ROUTINE( arccos,             PREFIXED(Acos) ),
  REXX_TYPED_ROUTINE( arcsin,             PREFIXED(Asin) ),
  REXX_TYPED_ROUTINE( arctan,             PREFIXED(Atan) ),

  REXX_TYPED_ROUTINE( atan2,              PREFIXED(Atan2) ),
  REXX_TYPED_ROUTINE( arctan2,            PREFIXED(Atan2) ),

  REXX_LAST_ROUTINE()
};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
RexxPackageEntry rxmpfr_package_entry =
{
  STANDARD_PACKAGE_HEADER
  REXX_INTERPRETER_5_0_0,
  "rxmpfr", "1.0.0",
  rxmpfrInit, rxmpfrTerm,
  rxmpfr_functions,
  NULL
} ;

OOREXX_GET_PACKAGE( rxmpfr ) ;

