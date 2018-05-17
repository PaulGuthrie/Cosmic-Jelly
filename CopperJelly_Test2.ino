#define USE_OCTOWS2811
#include "OctoWS2811.h"

#include "FastLED.h"

FASTLED_USING_NAMESPACE
////////////////////////////////////////// Copper Jelly v. 1.0 ///////////////////////////
// Code for the "Copper Jelly" sculpture
// Hardware is Teensy 3.2 and WS2812B LEDs
// Test version 2
// Paul Guthrie, May 2018
///////////////////////////////////////////////////////////////////////////////////////

// Small prototype for larger Burning Man sculpture

// forward function declarations////////////////////
void PulseAndGlitter() ;
void Pulse() ;
void ColorFountain() ;
void HSVCycle() ;
void BreatheFast(CRGB cfast) ;
void Breathe(CRGB color) ;
void MakePalettes() ;
void sinelon() ;
void Shadow() ;
void Snakes()  ;
void ColorRotate() ;
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount) ;
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount) ;
void fadeTowardColor( CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount) ;

///////////////////////////////////////////////////////
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif


// The leds are declared using the "one array, many strips" method and the OCTOWS2811 controller
// There are 4 strings, even though the controller defaults to 8 outputs --- we just ignore the last 4

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define NUM_LEDS_PER_STRIP   66
#define NUM_STRIPS            4
#define NUM_LEDS    NUM_STRIPS * NUM_LEDS_PER_STRIP
#define NUM_LEDS_PER_LEG     30
#define NUM_LEDS_PER_CAP     36

#define OFFSET_A    0
#define OFFSET_B    NUM_LEDS_PER_STRIP
#define OFFSET_C    2 * NUM_LEDS_PER_STRIP 
#define OFFSET_D    3 * NUM_LEDS_PER_STRIP

// define leds using sets
CRGBArray <NUM_LEDS> leds ;

// the structure of this sculpture is 4 helical legs, feeding 4 nested spirals, So the Leg sets
// are the helicals, and the Cap sets are the spirals wrapped around the...cap.
// The Strip sets reference each of the 4 combined strips. See FastLED doc for the addressing
// convention on the subsets of leds

CRGBSet CJStrip_A(leds( 0 , NUM_LEDS_PER_STRIP -1)) ;
CRGBSet CJStrip_B(leds (OFFSET_B, OFFSET_B + NUM_LEDS_PER_STRIP - 1 )) ;
CRGBSet CJStrip_C(leds (OFFSET_C, OFFSET_C + NUM_LEDS_PER_STRIP - 1 )) ;
CRGBSet CJStrip_D(leds (OFFSET_D, OFFSET_D + NUM_LEDS_PER_STRIP - 1 )) ;

CRGBSet CJLeg_A(leds(0 ,NUM_LEDS_PER_LEG - 1)) ;
CRGBSet CJLeg_B(leds(OFFSET_B, OFFSET_B + NUM_LEDS_PER_LEG - 1)) ;
CRGBSet CJLeg_C(leds(OFFSET_C, OFFSET_C + NUM_LEDS_PER_LEG - 1)) ;
CRGBSet CJLeg_D(leds(OFFSET_D, OFFSET_D + NUM_LEDS_PER_LEG - 1)) ;

CRGBSet CJCap_A(leds( 0 + NUM_LEDS_PER_LEG, NUM_LEDS_PER_STRIP - 1)) ;
CRGBSet CJCap_B(leds(OFFSET_B + NUM_LEDS_PER_LEG, 2 * NUM_LEDS_PER_STRIP -1 )) ;
CRGBSet CJCap_C(leds(OFFSET_C + NUM_LEDS_PER_LEG, 3 *  NUM_LEDS_PER_STRIP -1 )) ;
CRGBSet CJCap_D(leds(OFFSET_D + NUM_LEDS_PER_LEG, 4 * NUM_LEDS_PER_STRIP -1 )) ;

CHSVPalette16 currentPalette;
CRGBPalette16 currentRGBPalette ;
TBlendType    currentBlending;

CHSVPalette16 RainbowHSV_p ;
CHSVPalette16 BluesHSV_p ;
CHSVPalette16 RedsHSV_p ;
CHSVPalette16 GreensHSV_p ;

CRGBPalette16 ContrastStripes_p ;

#define BRIGHTNESS         192
#define FRAMES_PER_SECOND  120

// pattern duration in seconds
int duration = 120 ;

// waiting period in millis
int waitinit = 1000 ;
int wait = waitinit ;

// palette counter
int ipal = 0 ;

// animation counter
int anim = 0 ;
////////////////////////////////////
// enable power led on teensy
const int powerPin = 13 ;
////////////////////////////////////
  
void setup() {
  delay(3000); // 3 second delay for recovery
     Serial.begin(9600);
      Serial.println("Connected");
  pinMode (powerPin, OUTPUT) ;
  digitalWrite (powerPin, HIGH) ;
  
  // tell FastLED about the LED strip configuration, with multiple strips, one overall led array, using Octows2811
  
  FastLED.addLeds<OCTOWS2811>(leds,NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);

  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

    MakePalettes() ;

    currentPalette = RainbowHSV_p ; 
    currentRGBPalette = currentPalette ; 
}


  
void loop()
{
// the animations are self contained with timers, so no show in loop

// switch palette in HSV color space

      if(ipal == 0 ) {currentPalette = RainbowHSV_p ;}
      else if( ipal == 1) {currentPalette = BluesHSV_p ;}
        else if ( ipal == 2 ) {currentPalette = RedsHSV_p ;}
          else if( ipal == 3 ) {currentPalette = GreensHSV_p ;}
          
// we take advantage of the library's auto conversion of color space from 
// HSV to RGB and use a CRGB variable for the color, even though we are getting the color from
// an HSV palette
              
// switch palette in RGB color space          
       currentRGBPalette = currentPalette   ;
       
// begin patterns

     
      Pulse() ;   
      ColorFountain() ;
      PulseAndGlitter() ;
      for(int ic = 0 ; ic < 16 ; ic++ ) {
   
        CRGB c = currentRGBPalette[ic] ;
        Breathe(c) ; 
        EVERY_N_SECONDS(30) {
          BreatheFast(c) ;
          }       
    }
// One of the animations gets called, but different one on each pass through loop
      // select animation
      if(anim == 0 ) {sinelon() ;}
        else if( anim == 1) {Shadow() ;}
          else if ( anim == 2 ) {ColorRotate() ;}
            else if( anim == 3 ) {Snakes() ;}
     
      anim++ ;
      if( anim > 3 ) {anim = 0 ;} 
      
// update palette counter      
   ipal++ ;
   if( ipal > 3 ) {ipal = 0 ;} 
}

void Pulse() {
// modulates the brightness of entire sculpture
//controls
  int idir = 1 ;
  int cindex = 0 ;
  int Nsteps = 192 ;
  int DelB = 4 ;
// begin color loop
 for (int i = 0 ; i < 16 ; i++ ) {  
  CHSV c = currentPalette[cindex] ; 
  if( c.val < 255 ) {c.val = 255 ;}// make sure we are at full brightness
// begin brightness loop  
// The amount of change in brightness per step, DelB,  must be commensurate
// with the number of steps, Nsteps, to ensure that brightness cycles all
// the way back to 255 before the color changes. For the brightness
// range 64 to 255 (192 values) and 2 full cycles, the product
// DelB * Nsteps =  768
    for (int n = 0 ; n < Nsteps ; n++ ) {
      fill_solid(leds, NUM_LEDS, c) ;
      FastLED.show() ;
      FastLED.delay(40) ;
      c.val -= DelB*idir ;
      if( c.val <= 63 ) {idir = -idir ;}
      if ( c.val >= 254 ) {idir = -idir ;}
 }
   cindex ++ ;
   if (cindex > 15 ) {cindex = 0 ;}
 }
}

void PulseAndGlitter() {
// modulates the brightness of entire sculpture and adds white sparkles randomly
// addGlitter is taken directly from Mark Kriegsman's Github  

//controls
  int idir = 1 ;
  int cindex = 0 ;
  int Nsteps = 192 ;
  int DelB = 4 ;  
// begin color loop
 for (int i = 0 ; i < 16 ; i++ ) {  
  CHSV c = currentPalette[cindex] ; 
  if( c.val < 255 ) {c.val = 255 ;}// make sure we are at full brightness
// begin brightness loop  
   for (int n = 0 ; n < Nsteps ; n++ ) {   
      fill_solid(leds, NUM_LEDS, c) ;
      FastLED.show() ;
      FastLED.delay(40) ;
       addGlitter(90) ;
       c.val -= DelB * idir ;
       if( c.val <= 63 ) {idir = -idir ;}
      if ( c.val >= 254 ) {idir = -idir ;}
 }
   cindex ++ ;
   if (cindex > 15 ) {cindex = 0 ;}
 }
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] = CRGB::White ;
    FastLED.show() ; 
    FastLED.delay(50) ;  
  }
}

void HSVCycle() {
// function cycles brighness and saturation, changing color each time it 
// goes through black (val = 0) or white (sat = 0)

// color loop
  for( int ic = 0 ; ic < 8 ; ic += 2) {
    CHSV c1 = currentPalette[ic] ;
    CHSV c2 = currentPalette[ic + 1] ;
    
// brightness increse loop
    c1.val = 0 ;
    for(int is = 0 ; is < 15 ; is++ ) {
      fill_solid(leds , NUM_LEDS , c1) ;
      FastLED.show() ;
      FastLED.delay(1000) ;
      c1.val += 16 ;  
    }
// saturation decrease loop
        for(int is = 0 ; is < 15 ; is++ ) {
         fill_solid(leds , NUM_LEDS , c1) ;
         FastLED.show() ;
         FastLED.delay(1000) ;
         c1.sat -= 16 ;    
    }
// color change and saturation increase loop
    c2.sat = 0 ;
    for(int is = 0 ; is < 15 ; is++ ) {
        fill_solid(leds , NUM_LEDS , c2) ;
        FastLED.show() ;
        FastLED.delay(1000) ;
        c2.sat += 16 ;    
    }
// brightness decrease loop
    for(int is = 0 ; is < 15 ; is++ ) {
      fill_solid(leds , NUM_LEDS , c2) ;
      FastLED.show() ;
      FastLED.delay(1000) ;
      c2.val -= 16 ;    
    }        
// end color loop    
  }
// end of function  
}

void BreatheFast(CRGB cfast) {
// Breathe, but with an accelerating speed 
        while (wait > 20 ) {
          wait /= 2 ;
          Breathe(cfast) ;
        }
         wait = waitinit ;
}
void Breathe(CRGB color) {
//Inhales a color, then exhales it...with a wait interval between cycles
  
  fill_solid( leds, NUM_LEDS , CRGB::Black ) ;
  for (int i = 0 ; i < NUM_LEDS_PER_STRIP ; i++ ){
      CJStrip_A[i] = color ;
      CJStrip_B[i] = color ;
      CJStrip_C[i] = color ;
      CJStrip_D[i] = color ;   

        FastLED.show() ;
        FastLED.delay(wait / 30) ;    
  }
// change direction
  for (int i = NUM_LEDS_PER_STRIP -1 ; i >= 0 ; i-- ){
      CJStrip_A[i] = CRGB::Black ;
      CJStrip_B[i] = CRGB::Black ;
      CJStrip_C[i] = CRGB::Black ;
      CJStrip_D[i] = CRGB::Black ;   

        FastLED.show() ;
        FastLED.delay(wait / 30) ;    
  } 
      FastLED.delay(wait) ; 
}
void MakePalettes()
{
// sets up 4 HSV palettes  
  
// make the CHSV rainbow palette
   for( uint8_t j = 0; j < 16; j++) {
   uint8_t hue = j * 16;
   RainbowHSV_p[ j ] = CHSV( hue, 255, 255);
  }
  
// set up the fractional rainbow palettes
uint8_t rstart = 192 ;
uint8_t gstart = 64 ;
uint8_t bstart = 128 ;
 for( uint8_t j = 0; j < 16; j++) { 
  uint8_t rhue = rstart +  j * 5 % 255 ;
  uint8_t ghue = gstart +  j * 5 ;
  uint8_t bhue = bstart +  j * 5 ;
  RedsHSV_p[ j ] = CHSV( rhue, 255, 255);
  GreensHSV_p[ j ] = CHSV( ghue, 255, 255);
  BluesHSV_p[ j ] = CHSV( bhue, 255, 255);
 }
// set up contrasting stripes palette
  ContrastStripes_p = { CRGB::Blue , CRGB::Yellow , CRGB::Red, CRGB::Purple,
                  CRGB::Green , CRGB::Pink , CRGB::Teal , CRGB::Orange ,
                  CRGB::Cyan , CRGB::Magenta , CRGB::DarkBlue, CRGB::Gold,
                  CRGB::Violet , CRGB::SeaGreen , CRGB::DarkRed , CRGB::Gray  } ; 
}

void sinelon()
{

// this is from the demoreel example, adapted to run on 4 strips at once
int xHue = 0 ;
long starttime = millis() ;
long nowtime = starttime ;
  while ( (nowtime - starttime) < duration * 1000L ) 
  {
    EVERY_N_MILLISECONDS( 40 ) { xHue++; } // slowly cycle the "base color" through the rainbow  
    fadeToBlackBy( leds, NUM_LEDS, 20);
    int pos = beatsin16(16,0,NUM_LEDS_PER_STRIP - 1);
    leds[pos] += CHSV( xHue, 255, 192);
    leds[pos + NUM_LEDS_PER_STRIP] += CHSV(xHue, 255, 192) ;
    leds[pos + 2 * NUM_LEDS_PER_STRIP] += CHSV(xHue, 255, 192) ;
    leds[pos + 3 * NUM_LEDS_PER_STRIP] += CHSV(xHue, 255, 192) ;
  
     // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(800/FRAMES_PER_SECOND);
   
  nowtime = millis() ;
  }
}

void Shadow()
{
// creates a low brightness shadow moving across the leds

// loop over current palette
  for(int ihue = 0 ; ihue < 16 ; ihue++ ) {  
// Get the current color and brightness

CHSV c = currentPalette[ihue] ; 
int bri = BRIGHTNESS ;

fill_solid (leds, NUM_LEDS, c) ;

int shadowpos = 0 ;            // shadow position on strip
int shadowpad = 8 ;            // additional shadow padding

// upward loop
  for(int i = 0 ; i < NUM_LEDS_PER_STRIP  - shadowpad ; i++ ) {
//      c.val = bri / 16 ;    // use this for partial shadow
        c.val = 0 ;           // full shadow
      shadowpos = i;
      for(int j= shadowpos ; j < shadowpos + shadowpad ; j++) {
      CJStrip_A[j] = c ;
      CJStrip_B[j] = c ;
      CJStrip_C[j] = c ;
      CJStrip_D[j] = c ; 
      }
    FastLED.show() ;

    FastLED.delay(75) ;
  
// reset  to full brightness  
    c.val = bri ;
    fill_solid( leds, NUM_LEDS, c) ;
    }
// downward loop
      for(int i = NUM_LEDS_PER_STRIP - 1 - shadowpad ; i >= 0 ; i-- ) {
//      c.val = bri / 16 ;
        c.val = 0 ;
      shadowpos =  i ;
      for(int j= shadowpos ; j < shadowpos + shadowpad ; j++) {
      CJStrip_A[j] = c ;
      CJStrip_B[j] = c ;
      CJStrip_C[j] = c ;
      CJStrip_D[j] = c ; 
      }
  
    FastLED.show() ;

    FastLED.delay(75) ;
  
// reset to full brightness  
    c.val = bri ;
    fill_solid( leds, NUM_LEDS , c) ;
    }
  }

}

void Snakes() 
{
 // This function sets colors in both HSV and RGB spaces. The duplication is because we are using CHSV colors to
 // allow the hue to define a complementary color, but we want to blend the complement back toward
 // the background. Color blending only works in RGB.
 

  int ileg = 0 ;
  int snakeoffset = 0 ;
  
for( int ihue = 0 ; ihue < 16 ; ihue++) 
{
// update backcolor from palette ...wrapping loop
// set background color
// and a copy converted to RGB space
  CHSV backcolor = currentPalette[ihue] ;
  CRGB bcrgb = backcolor ;
  
// fill the background
  fill_solid (leds, NUM_LEDS, CRGB::Black ) ;
  
  for ( int i =0 ; i < NUM_LEDS_PER_STRIP ; i++ ) {
   leds[i] = backcolor ;
   leds[i + NUM_LEDS_PER_STRIP] = backcolor ;
   leds[i + 2 * NUM_LEDS_PER_STRIP] = backcolor ;
   leds[i + 3 * NUM_LEDS_PER_STRIP] = backcolor ; 
    FastLED.show() ;

    FastLED.delay(1500/FRAMES_PER_SECOND ) ;
  }
// define snake color & converted copy
    CHSV snakecolor = backcolor ;
    snakecolor.hue = (backcolor.hue + 128); 
    CRGB scrgb = snakecolor ;
// select a leg 
    snakeoffset = ileg * NUM_LEDS_PER_STRIP ;
    for (int i = 0 ; i < NUM_LEDS_PER_STRIP ; i++ ) {
      leds[i + snakeoffset] = snakecolor ;
      ileg++ ;
      if (ileg > 3) {ileg = 0 ; }
      FastLED.show() ;
      FastLED.delay (1500/FRAMES_PER_SECOND ) ;
    }
// Fade snake to background using the RGB versions of the colors
int nsteps = 4 ;
  for(int n = 0 ; n < nsteps ; n++ ) {
    fadeTowardColor (scrgb, bcrgb, 512 / nsteps) ;
    for (int i = 0 ; i < NUM_LEDS_PER_STRIP ; i++ ) {
      leds[i + snakeoffset] = scrgb ;
      FastLED.show() ;
      FastLED.delay (1500/FRAMES_PER_SECOND ) ;
     }
    }
  }
}
//Next section borrowed from MK  
// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
  if( cur == target) return;
  
  if( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor( CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount)
{
  for( uint16_t i = 0; i < N; i++) {
    fadeTowardColor( L[i], bgColor, fadeAmount);
  }
}

void ColorRotate() 
{
// function gets a color from the current palette, rotates it through the four strips
//repeats for the specified duration
  

  int ihue = 0 ;

  fill_solid(leds, NUM_LEDS, CRGB::Black) ;
  CRGB c1 = CRGB::Black ;
  CRGB c2 = CRGB::Black ;
  CRGB c3 = CRGB::Black ;
  CRGB c4 = CRGB::Black ;
  
  long starttime = millis() ;
  long nowtime = starttime ;
  while (nowtime - starttime < duration * 1000L ) 
    {
      c1 = currentPalette[ihue] ;
      
      fill_solid(CJStrip_A, NUM_LEDS_PER_STRIP , c1) ;
      fill_solid(CJStrip_B, NUM_LEDS_PER_STRIP , c2) ;
      fill_solid(CJStrip_C, NUM_LEDS_PER_STRIP , c3) ;
      fill_solid(CJStrip_D, NUM_LEDS_PER_STRIP , c4) ;

      FastLED.show() ;
      FastLED.delay(300) ;

// include these lines to make it blink on and off /////////
//      fill_solid(leds, NUM_LEDS , CRGB::Black) ;

//      FastLED.show() ;
//      FastLED.delay(100) ;
///////////////////////////////////////////////////////////

// update colors and index, check index limit and wrap around.
//
      c4 = c3 ;
      c3 = c2 ;
      c2 = c1 ;
      
      ihue++ ;
      if (ihue > 15) { ihue = 0 ;}
// update elapsed time
      nowtime = millis() ;            
    }
  
}

void ColorFountain() 
{
// function produces a specified number of bands of color that move sequentially up the legs
// and into the cap. N_BANDS must be an integral divisor of NUM_LEDS_PER_LEG.  We keep track of the position
// of the top of each band and the current color index in two arrays. Band position is updated on each
// pass through the timing loop, and colors are updated each time a band reaches the top. Color is pulled
// from currentPalette according to the color index in Bcolor

    int N_BANDS = 6 ;
    int Bwidth = 5 ; //NUM_LEDS_PER_LEG / N_BANDS ;
    int Btops[N_BANDS] ;
    int Bcolor[N_BANDS] ;

// initialization
// the bands are stacked in negative location space and move up one place each time through 
// the loop. When each one clears the top of the leg its top resets to location -1
    for(int j = 0 ; j < N_BANDS ; j++ ) 
    { Btops[j] = - (j * Bwidth + 1)  ;
      Bcolor[j] = j ;
    }
    fill_solid(leds, NUM_LEDS, CRGB::Black) ;
  int counter =0 ;  
  long starttime = millis() ;
  long nowtime = starttime ;
  
  while (nowtime - starttime < duration * 1000L ) {
    
// switch palette halfway through
  if( nowtime - starttime >= duration * 1000L / 2 ) {currentRGBPalette = ContrastStripes_p ;}   

// move the tops of the bands up one position.   

    for(int j = 0 ; j < N_BANDS ; j++ ) {
      Btops[j] += 1 ;
      for (int i = 0 ; i < Bwidth ; i++ ) {
// check range to avoid overwriting something as the band passes through the top of the leg  
      int iband = Btops[j] - i ;         
        if ( (iband >= 0) && (iband < NUM_LEDS_PER_LEG)  ) {
// write the color for this band to the positions on each of the four legs 
      int bc = Bcolor[j] % 15 ;
        CRGB bandcolor = currentRGBPalette[bc] ; 

         CJLeg_A[ iband ] = bandcolor ;
          CJLeg_B[ iband ] = bandcolor ;
          CJLeg_C[ iband ] = bandcolor ;
          CJLeg_D[ iband ] = bandcolor ;  
          }                            
        }
      }

// see if a band has passed the top of the legs
       for (int j = 0 ; j < N_BANDS ; j++ ) {
           
        if ( ( Btops[j] - (Bwidth - 1 ) )== NUM_LEDS_PER_LEG - 1 ) {
          int icc = Bcolor[j] % 15 ;
          CRGB capcolor = currentPalette[icc] ;
          fill_solid(CJCap_A, NUM_LEDS_PER_CAP , capcolor ) ;
          fill_solid(CJCap_B, NUM_LEDS_PER_CAP , capcolor ) ;
          fill_solid(CJCap_C, NUM_LEDS_PER_CAP , capcolor ) ;
          fill_solid(CJCap_D, NUM_LEDS_PER_CAP , capcolor ) ;                              
          Btops[j] = -1 ;
          Bcolor[j]+= N_BANDS - 1 ;
//          if(Bcolor[j] > 15 ) {Bcolor[j] = 0 ;}         
          }
          
        }
          FastLED.show() ;
         FastLED.delay(40) ;
        nowtime = millis() ;
    }
// reset palette from ContrastStripes to the current palette
      currentRGBPalette = currentPalette ;
  }


