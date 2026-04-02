// From: https://github.com/usolved/cie-rgb-converter/blob/master/cie_rgb_converter.js
/*
 * Converts RGB color space to CIE color space
 * @param {Number} red
 * @param {Number} green
 * @param {Number} blue
 * @return {Array} Array that contains the CIE color values for x and y
 */

 struct myVector{
  float x;
  float y;
  void mul(float coef){
    x = x*coef;
    y = y*coef;
  }
  void div(float coef){
    x = x/coef;
    y = y/coef;
  }

  void subtract(struct myVector v){
    x = x- v.x;
    y = y- v.y;
  }
  void add(struct myVector v){
    x = x+ v.x;
    y = y+ v.y;
  }
 };

 float getDeltaTime(){
  float currentTime = millis();
  float deltaTime = currentTime - oldTime;
  oldTime = currentTime;
  return deltaTime;
}

 struct colorXY myCIE(struct colorRGB rgb){
  //https://viereck.ch/hue-xy-rgb/
  myVector vCenter = {0.313,0.329};

  //myVector vr = {0.64,0.33};
  myVector vr = {0.52,0.33}; //adjust red to make it white white
  myVector vg = {0.3,0.6};
  myVector vb = {0.15,0.06};

  float rRatio = float(rgb.r)/255.0;
  float gRatio = float(rgb.g)/255.0;
  float bRatio = float(rgb.b)/255.0;

  vr.subtract(vCenter);
  vg.subtract(vCenter);
  vb.subtract(vCenter);

  vr.mul(rRatio);
  vg.mul(gRatio);
  vb.mul(bRatio);

  vCenter.add(vr);
  vCenter.add(vg);
  vCenter.add(vb);

  //https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
  float luminance = (0.2126*rRatio+ 0.7152*gRatio + 0.0722*bRatio) * 100.0;

  colorXY xy = {vCenter.x,vCenter.y,0,luminance};

  return xy;
 }

 float degToRad(float deg){
  float rad = (deg * 71) / 4068;
  return rad;
}

struct colorRGB bleathSin(struct colorRGB rgb,int deg){

  float degrees = deg;
  float radians = degToRad(degrees);
  float ratio = (sin(radians) +1)/2.0;
  if(ratio<0){
    ratio = 0;
  }

  rgb.r = round(float(rgb.r)*ratio);
  rgb.g = round(float(rgb.g)*ratio);
  rgb.b = round(float(rgb.b)*ratio);
  return rgb;
}

struct colorXY bleathSin2(struct colorRGB rgb,int deg){

  float degrees = deg;
  float radians = degToRad(degrees);
  float ratio = (sin(radians) +1)/2.0;
  if(ratio<0){
    ratio = 0;
  }

  struct colorXY xy;

  xy = myCIE(rgb);


  xy.brightness = xy.brightness*ratio;

  return xy;
}

struct colorXY bleathSin3(struct colorRGB rgb,float deg){

  float degrees = deg;
  float radians = degToRad(degrees);
  float ratio = (sin(radians) +1)/2.0;
  if(ratio<0){
    ratio = 0;
  }

  struct colorXY xy;

  xy = myCIE(rgb);


  xy.brightness = xy.brightness*ratio;

  return xy;
}

struct colors bleathSin4(struct colorRGB rgb,float deg){

  float degrees = deg;
  float radians = degToRad(degrees);
  float ratio = (sin(radians) +1)/2.0;
  if(ratio<0){
    ratio = 0;
  }

  struct colors cs;

  cs.xy = myCIE(rgb);
  cs.rgb.r = round(25+float(rgb.r)*ratio*0.9);
  cs.rgb.g = round(25+float(rgb.g)*ratio*0.9);
  cs.rgb.b = round(25+float(rgb.b)*ratio*0.9);
  
  cs.xy.brightness = cs.xy.brightness*ratio;

  return cs;
}

 struct colorXY rgb_to_cie(struct colorRGB rgb){

  float fr = float(rgb.r);
  float fg = float(rgb.g);
  float fb = float(rgb.b);

  /*
	//Apply a gamma correction to the RGB values, which makes the color more vivid and more the like the color displayed on the screen of your device
	float red 	= (fr > 0.04045) ? pow((fr + 0.055) / (1.0 + 0.055), 2.4) : (fr / 12.92);
  float green 	= (fg > 0.04045) ? pow((fg + 0.055) / (1.0 + 0.055), 2.4) : (fg/ 12.92);
  float blue 	= (fb > 0.04045) ? pow((fb  + 0.055) / (1.0 + 0.055), 2.4) : (fb  / 12.92); 
  */

  float red = fr;
  float green = fg;
  float blue = fb;

  //red = red*0.8; //adjust to set white to white

	//RGB values to XYZ using the Wide RGB D65 conversion formula
  float X 		= red * 0.664511 + green * 0.154324 + blue * 0.162028;
  float Y 		= red * 0.283881 + green * 0.668433 + blue * 0.047685;
  float Z 		= red * 0.000088 + green * 0.072310 + blue * 0.986039;

	//Calculate the xy values from the XYZ values
	float x 		= (X / (X + Y + Z));
	float y 		= (Y / (X + Y + Z));
  float z  = Z;
  float brightness = Z/555623.12*100.0;

  x =  round(x*1000.0)/1000.0;
  y =  round(y*1000.0)/1000.0;
  z =  round(y*1000.0)/1000.0;

	if (isnan(x))
		x = 0;
	if (isnan(y))
		y = 0;	 
	if (isnan(z)){
		z = 0;	
    brightness = 0;
  }


  struct colorXY xy;
  xy.x = x;
  xy.y = y;
  xy.z = z;
  xy.brightness = brightness;

	return xy;
}