/*  conversion factors from meters
*/
.local~kmConv = 1000.0
.local~smConv = 1609.344
.local~nmConv = 1852.0

/*  WGS84 ellipsoid hearth model
*/
.local~equat_radius = 6378137.0
.local~polar_radius = 6356752.3142
.local~earth_radius = ( .equat_radius + .equat_radius + .polar_radius) / 3

.local~inverseFlattening = 298.257223563
.local~flattening = 1 / 298.257223563

/*
say "equat_radius " .equat_radius
say "flattening   " .flattening
say "polar_radius " .equat_radius * ( 1 - .flattening )
*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine normalize public
    use strict arg v, min=0., max = 360.

    r = v

    if r > max  | v = low then ,
        v = min + abs( v + max ) // ( abs(min) + abs(max) )

    if r < min | v =  max then ,
        v = max - abs( v - min ) // ( abs(min) + abs(max) )

    if v = upper then ,
        return min

    return v

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    haversine formula
*/
::routine greatCircleAngle  public
    -- say "haversine formula "
    -- say
    use strict arg pt1, pt2

    cos_lat1 = cos(pt1[1])
    cos_lat2 = cos(pt2[1])

    dlat =  pt2[1] - pt1[1]
    dlng =  pt2[2] - pt1[2]

    a = sin( dlat/2 ) * sin( dlat/2 ) + ,
        sin( dlng/2 ) * sin( dlng/2 ) * cos_lat1 * cos_lat2

    return 2 * atan2( sqrt(a) ,sqrt(1-a) )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    spherical law of cosines
*/
::routine greatCircleAngle_loc  public
    -- say "spherical law of cosines "
    -- say
    use strict arg pt1, pt2

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])
    sin_lat2 = sin(pt2[1]); cos_lat2 = cos(pt2[1])

    dlat =  pt2[1] - pt1[1]
    dlng =  pt2[2] - pt1[2]

    return acos( sin_lat1 * sin_lat2 + ,
                 cos_lat1 * cos_lat2 * cos( dlng ) )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    chord length method
*/
::routine greatCircleAngle_clm  public
    -- say "chord length method "
    -- say
    use strict arg pt1, pt2

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])
    sin_lat2 = sin(pt2[1]); cos_lat2 = cos(pt2[1])
    sin_lng1 = sin(pt1[2]); cos_lng1 = cos(pt1[2])
    sin_lng2 = sin(pt2[2]); cos_lng2 = cos(pt2[2])

    d_x =  cos_lat2 * cos_lng2 - cos_lat1 * cos_lng1
    d_y =  cos_lat2 * sin_lng2 - cos_lat1 * sin_lng1
    d_z =  sin_lat2 - sin_lat1

    return 2 * asin( sqrt( d_x*d_x + d_y*d_y + d_z*d_z ) / 2 )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Vincenty special case
*/
::routine greatCircleAngle_Vinspec public
    -- say "Vincenty special case "
    -- say
    use strict arg pt1, pt2

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])
    sin_lat2 = sin(pt2[1]); cos_lat2 = cos(pt2[1])

    sin_dlng = sin( pt2[2] - pt1[2] )
    cos_dlng = cos( pt2[2] - pt1[2] )

    /*
    y = sqrt( ( cos_lat2 * sin_dlng ) ** 2 + ,
              ( cos_lat1 * sin_lat2 - ,
                sin_lat1 * cos_lat2 * cos_dlng ) ** 2 )
    */
    y = hypot( ( cos_lat2 * sin_dlng ), ,
               ( cos_lat1 * sin_lat2 - sin_lat1 * cos_lat2 * cos_dlng ) )

    x = sin_lat1 * sin_lat2 + cos_lat1 * cos_lat2 * cos_dlng

    return atan2( y, x )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine greatCircleDistance public
    use strict arg pt1, pt2

    return  .earth_radius * radians( greatCircleAngle( pt1, pt2 ) )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine initialBearing public
    use strict arg pt1, pt2

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])
    sin_lat2 = sin(pt2[1]); cos_lat2 = cos(pt2[1])

    sin_dlng = sin( pt2[2] - pt1[2] )
    cos_dlng = cos( pt2[2] - pt1[2] )

    y = cos_lat2 * sin_dlng
    x = cos_lat1 * sin_lat2 - sin_lat1 * cos_lat2 * cos_dlng

    c = atan2( y, x )

    return normalize( c )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine finalBearing public
    use strict arg pt1, pt2

    c = initialBearing( pt2, pt1 ) + 180

    return normalize( c )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine finalBearing_native public
    use strict arg pt1, pt2

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])
    sin_lat2 = sin(pt2[1]); cos_lat2 = cos(pt2[1])

    --  NOTE delta is inverted
    sin_dlng = sin( pt1[2] - pt2[2] )
    cos_dlng = cos( pt1[2] - pt2[2] )

    y = cos_lat1 * sin_dlng
    x = cos_lat2 * sin_lat1 - sin_lat2 * cos_lat1 * cos_dlng

    c = atan2( y, x ) + 180

    return normalize( c )

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine middlePoint public
    use strict arg pt1, pt2

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])
    sin_lat2 = sin(pt2[1]); cos_lat2 = cos(pt2[1])

    sin_dlng = sin( pt2[2] - pt1[2] )
    cos_dlng = cos( pt2[2] - pt1[2] )

    b_x = cos_lat2 * cos_dlng
    b_y = cos_lat2 * sin_dlng

    /*
    lat3  = atan2( sin_lat1 + sin_lat2, ,
                   sqrt( ( cos_lat1 + b_x ) * ( cos_lat1 + b_x ) + b_y * b_y) )
    */
    lat3  = atan2( ( sin_lat1 + sin_lat2 ), ,
                   hypot( cos_lat1 + b_x , b_y ) )

    lng3 =  pt1[2] + atan2( b_y ,( cos_lat1 + b_x ) )

    return ( lat3, normalize( lng3, -180, 180 ) )

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine fractionalPoint public
    use strict arg pt1, pt2, fract=0.5

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])
    sin_lat2 = sin(pt2[1]); cos_lat2 = cos(pt2[1])
    sin_lng1 = sin(pt1[2]); cos_lng1 = cos(pt1[2])
    sin_lng2 = sin(pt2[2]); cos_lng2 = cos(pt2[2])

    angle = greatCircleAngle( pt1, pt2 )
    sin_angl = sin(angle)

    a = sin( (1-fract) * angle ) / sin_angl
    b = sin(    fract  * angle ) / sin_angl

    x = a * cos_lat1 * cos_lng1 + b * cos_lat2 * cos_lng2
    y = a * cos_lat1 * sin_lng1 + b * cos_lat2 * sin_lng2
    z = a * sin_lat1 + b * sin_lat2

    lat3 = atan2( z, hypot( x, y ) )

    lng3 = atan2( y, x )

    return ( lat3, lng3 )

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine enroutePoint  public
    use strict arg pt1, brng, dist

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])
    sin_lng1 = sin(pt1[2]); cos_lng1 = cos(pt1[2])

    sin_brng = sin(brng); cos_brng = cos(brng)

    angle = degrees( dist / .earth_radius )
    sin_angl = sin( angle )
    cos_angl = cos( angle )

    sin_lat2 =   sin_lat1 * cos_angl + cos_lat1 * sin_angl * cos_brng

    lat2 = asin( sin_lat2 )

    y = sin_brng * sin_angl * cos_lat1
    x = cos_angl - sin_lat1 * sin_lat2

    lng2 = pt1[2] + atan2( y, x );

    return ( lat2, lng2 )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine maxLatitude       public
    use strict arg pt1, brng

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])

    brng = normalize( brng, -180.0, 180.0)
    sin_brng = sin(brng); cos_brng = cos(brng)

    lat2 = acos( abs( sin_brng * cos_lat1 ) )
    if  brng < -90 | brng > 90 then ,
        lat2 = -lat2

    return lat2

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine longitudeAtMaxlatitude    public
    use strict arg pt1, brng
    lat1 = pt1[1]; lng1 = pt1[2]
    brng = normalize( brng, -180.0, 180.0)

    lng2 = lng1 + atan2( 1.0, tan(brng) * sin(lat1) )

    return lng2-180.0*(brng<0.0)

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::routine closestPointToPole      public
    use strict arg pt1, brng

    sin_lat1 = sin(pt1[1]); cos_lat1 = cos(pt1[1])

    brng = normalize( brng, -180.0, 180.0)
    sin_brng = sin(brng); cos_brng = cos(brng); tan_brng = tan(brng)

    lat2 = acos( abs( sin_brng * cos_lat1 ) )
    if  brng < -90 | brng > 90 then ,
        lat2 = -lat2

    lng2 = pt1[2] + atan2( 1.0, tan_brng * sin_lat1 )

    return ( lat2, lng2-180.0*(brng<0.0) )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::options   digits 36

::requires  "rxmpfr" library








