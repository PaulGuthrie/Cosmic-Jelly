#include "FastLED.h"

FASTLED_USING_NAMESPACE
//////////////////////////////////////////Cosmic Jelly v. 3.0 ///////////////////////////
// Code for the "Cosmic Jelly" sculpture
// Hardware is Teensy 3.2 and APA102 LEDs
// Test version 3
// Paul Guthrie, july 2017


// forward function declarations////////////////////
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

// The sculpture uses 4 separate strings of leds with 4 data lines and a shared clock pin
// The leds are declared using the "one array, many strips" method

#define DATA_PIN_A    22
#define DATA_PIN_B    21
#define DATA_PIN_C    20
#define DATA_PIN_D    6

#define CLK_PIN     14
#define LED_TYPE    APA102
#define COLOR_ORDER BGR

#define NUM_LEDS_PER_STRIP   174
#define NUM_STRIPS            4
#define NUM_LEDS    NUM_STRIPS * NUM_LEDS_PER_STRIP

#define OFFSET_A    0
#define OFFSET_B    NUM_LEDS_PER_STRIP
#define OFFSET_C    2 * NUM_LEDS_PER_STRIP 
#define OFFSET_D    3 * NUM_LEDS_PER_STRIP

// define leds using sets
CRGBArray <NUM_LEDS> leds ;

// the structure of this sculpture is 4 helical legs, feeding 4 nested spirals, So the Leg sets
// are the helicals, and the Cap sets are the spirals wrapped around the...cap.
// The Strip sets reference each of the 4 combined strips

CRGBSet CJStrip_A(leds( 0 , NUM_LEDS_PER_STRIP -1)) ;
CRGBSet CJStrip_B(leds (OFFSET_B, OFFSET_B + NUM_LEDS_PER_STRIP - 1 )) ;
CRGBSet CJStrip_C(leds (OFFSET_C, OFFSET_C + NUM_LEDS_PER_STRIP - 1 )) ;
CRGBSet CJStrip_D(leds (OFFSET_D, OFFSET_D + NUM_LEDS_PER_STRIP - 1 )) ;

CRGBSet CJLeg_A(leds(0,73)) ;
CRGBSet CJLeg_B(leds(NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP + 73)) ;
CRGBSet CJLeg_C(leds(2 * NUM_LEDS_PER_STRIP, 2 *  NUM_LEDS_PER_STRIP + 73)) ;
CRGBSet CJLeg_D(leds(3 * NUM_LEDS_PER_STRIP, 3 * NUM_LEDS_PER_STRIP + 73)) ;

CRGBSet CJCap_A(leds(74, NUM_LEDS_PER_STRIP )) ;
CRGBSet CJCap_B(leds(NUM_LEDS_PER_STRIP + 74, NUM_LEDS_PER_STRIP )) ;
CRGBSet CJCap_C(leds(2 * NUM_LEDS_PER_STRIP + 74, 2 *  NUM_LEDS_PER_STRIP )) ;
CRGBSet CJCap_D(leds(3 * NUM_LEDS_PER_STRIP + 74, 3 * NUM_LEDS_PER_STRIP )) ;

CHSVPalette16 currentPalette;
CRGBPalette16 currentRGBPalette ;
TBlendType    currentBlending;

CHSVPalette16 RainbowHSV_p ;
CHSVPalette16 BluesHSV_p ;
CHSVPalette16 RedsHSV_p ;
CHSVPalette16 GreensHSV_p ;

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
  
void setup() {
  delay(3000); // 3 second delay for recovery
//     Serial.begin(9600);
//      Serial.println("Connected");
  
  // tell FastLED about the LED strip configuration, with 4 separate strips, one overall led array
  
  FastLED.addLeds<LED_TYPE,DATA_PIN_A,CLK_PIN,COLOR_ORDER, DATA_RATE_MHZ(2) >(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<LED_TYPE,DATA_PIN_B,CLK_PIN,COLOR_ORDER, DATA_RATE_MHZ(2) >(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<LED_TYPE,DATA_PIN_C,CLK_PIN,COLOR_ORDER, DATA_RATE_MHZ(2) >(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<LED_TYPE,DATA_PIN_D,CLK_PIN,COLOR_ORDER, DATA_RATE_MHZ(2) >(leds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

    MakePalettes() ;

    currentPalette = RainbowHSV_p ;  
}


  
void loop()
{
// the animations are self contained with timers, so no show in loop

// switch palette

      if(ipal == 0 ) {currentRGBPalette = PartyColors_p ;}
      else if( ipal == 1) {currentRGBPalette = BluesHSV_p ;}
        else if ( ipal == 2 ) {currentRGBPalette = RedsHSV_p ;}
          else if( ipal == 3 ) {currentRGBPalette = GreensHSV_p ;}
      
      for(int ic = 0 ; ic < 16 ; ic++ ) {
// we take advantage of the library's auto conversion of color space from 
// HSV to RGB and use a CRGB variable for the color, even though we are getting the color from
// an HSV palette        
        CRGB c = currentRGBPalette[ic] ;
        Breathe(c) ; 
        EVERY_N_SECONDS(30) {
          BreatheFast(c) ;
          } 
      
// One of the different animations gets called every 5 minutes
      EVERY_N_MINUTES(5) {
      // select animation
      if(anim == 0 ) {sinelon() ;}
        else if( anim == 1) {Shadow() ;}
          else if ( anim == 2 ) {ColorRotate() ;}
            else if( anim == 3 ) {Snakes() ;}
     
      anim++ ;
      if( anim > 3 ) {anim = 0 ;} 
      }
    }
// update palette counter      
   ipal++ ;
   if( ipal > 3 ) {ipal = 0 ;} 
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
   RainbowHSV_p[ j ] = CHSV( hue, 255, BRIGHTNESS);
  }
  
// set up the fractional rainbow palettes
uint8_t rstart = 192 ;
uint8_t gstart = 64 ;
uint8_t bstart = 128 ;
 for( uint8_t j = 0; j < 16; j++) { 
  uint8_t rhue = rstart +  j * 5 % 255 ;
  uint8_t ghue = gstart +  j * 5 ;
  uint8_t bhue = bstart +  j * 5 ;
  RedsHSV_p[ j ] = CHSV( rhue, 255, BRIGHTNESS);
  GreensHSV_p[ j ] = CHSV( ghue, 255, BRIGHTNESS);
  BluesHSV_p[ j ] = CHSV( bhue, 255, BRIGHTNESS);
 }
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
      FastLED.delay(200) ;

// include these lines to make it blink on and off /////////
//      fill_solid(leds, NUM_LEDS , CRGB::Black) ;

//      FastLED.show() ;
//      FastLED.delay(100) ;
///////////////////////////////////////////////////////////

// update colors and index, check index limit and wrap around
      c4 = c3 ;
      c3 = c2 ;
      c2 = c1 ;
      ihue++ ;
      if (ihue > 15) { ihue = 0 ;}
// update elapsed time
      nowtime = millis() ;            
    }
  
}


