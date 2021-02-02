#ifndef HM_MATHS_HPP
#define HM_MATHS_HPP

/*
   modulo that handles correctly negative numbers (only returns positives values)
   Not tested on compiler that already handle florring of negative numbers correctly but I guess it shall work too ...
*/
int hm_mod(int a, int b);
/*
   division that handle correctly negative number (floors to the greatest int lower than or equals to the real value)
   We check the compiler dependent modulo behavior to guess the floring of the corresponding division
   Not tested on compiler that already handle florring of negative numbers correctly but I guess it shall work too ...
*/
int hm_div(int a, int b);

/*Puf fft and hook processing somewhere ?*/

#endif // HM_MATHS_HPP
