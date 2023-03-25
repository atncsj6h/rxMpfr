#!/usr/bin/env rexx

say "bits/digits" config()[1] config()[2]


say "equator radius(Km)     " format( .equat_radius/.kmConv, ,6)
say "inverse Flattening     " format( .inverseFlattening, ,16)
say "flattening             " format( .flattening, ,16)
say "polar radius (Km)      " format( .polar_radius/.kmConv, ,6 )
say "avg earth radius (Km)  " format( .earth_radius/.kmConv, ,6 )
say "avg earth radius (Nm)  " format( .earth_radius/.nmConv, ,6 )
say


pt1 = ( 036.12,  -86.67 )
pt2 = ( 033.94, -118.40 )

/*
036  7.2,  -086 40.2
033 56.4,  -118 24.0
*/

say "(BNA) Nashville        " DMS( pt1[1] ) DMS( pt1[2] )
say "(LAX) Los Angeles      " DMS( pt2[1] ) DMS( pt2[2] )

say "haversine              " DMS( greatCircleAngle( pt1, pt2 ) )          greatCircleAngle( pt1, pt2 )
say "law of cosines         " DMS( greatCircleAngle_loc( pt1, pt2 ) )      greatCircleAngle_loc( pt1, pt2 )
say "chord length           " DMS( greatCircleAngle_clm( pt1, pt2 ) )      greatCircleAngle_clm( pt1, pt2 )
say "Vincenty special       " DMS( greatCircleAngle_Vinspec( pt1, pt2 ) )  greatCircleAngle_Vinspec( pt1, pt2 )
say

dist1 = greatCircleDistance( pt1, pt2 )
say "distance ( km)          " format( dist1/.kmConv, ,4 )
say "distance ( sm)          " format( dist1/.smConv, ,4 )
say "distance ( Nm)          " format( dist1/.nmConv, ,4 )
say

brng1 = initialBearing( pt1, pt2 )
brng2 = finalBearing( pt1, pt2 )
say "initial bearing        " DMS(brng1)
say "final   bearing        " DMS( finalBearing( pt1, pt2 ) )
say "                       " DMS( finalBearing_native( pt1, pt2 ) )
say

say "on a great circle route"
say "   the product sin(bearing)*cos(latitude)"
say "   is constant"
say "sin(brng1)*cos(pt1[1]) " sin(brng1)*cos(pt1[1])
say "sin(brng2)*cos(pt2[1]) " sin(brng2)*cos(pt2[1])
say

pt3 = middlePoint( pt1, pt2 )
say "middle point           " DMS(pt3[1]) DMS(pt3[2])
pt3 = fractionalPoint( pt1, pt2 , 0.5 )
say "fractional at 0.5      " DMS(pt3[1]) DMS(pt3[2])
say

say "first leg (dist/10)"
say "origin                 " DMS(pt1[1]) DMS(pt1[2])
pt3 = enroutePoint( pt1 , brng1, dist1/10 )
say "destination            " DMS(pt3[1]) DMS(pt3[2])
angl = greatCircleAngle( pt1, pt3 )
say "angular distance       " DMS(angl)
dist = greatCircleDistance( pt1, pt3 )
say "distance ( km)          " format( dist/.kmConv, ,4 )
brng = initialBearing( pt1, pt3 )
say "initial bearing        " DMS(brng)
say "final   bearing        " DMS(finalBearing( pt1, pt3 ) )
say

lat3 = maxLatitude( pt1, initialBearing( pt1, pt2 ) )
say "max latitude           " DMS( lat3 )
lng3 = longitudeAtMaxlatitude( pt1, initialBearing( pt1, pt2 ) )
say "lng at max latitude    " DMS( lng3 )
say

pt3 = closestPointToPole( pt1, initialBearing( pt1, pt2 ) )
say "closestPointToPole     " DMS(pt3[1]) DMS(pt3[2])

exit

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
DMS:    procedure
    use strict arg deg
    if  deg < 0 then do
        neg = "-"
        deg = -deg
    end
    else ,
        neg = " "

    min = ( deg // 1 ) * 60
    deg = deg % 1

    sec = format( ( min // 1 ) * 60, , 0)
    min = min % 1

    if  min = 60 then do
        deg += 1
        min = 0
    end
    if  sec = 60 then do
        min += 1
        sec  = 0
    end

    return neg || right(deg,3,"0")":"right(min,2,"0")":"right(sec,2,"0")""

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::options   novalue condition
::options   digits 36

::requires  "navmath.cls"
::requires  "rxmpfr" library

