/*
     Automatic Ball Tracking
     Mechatronics Lab 4
     Max Novick
     Rachel Yung
     2/17/2016
*/

#include <SPI.h>
#include <Pixy.h>

Pixy pixy;

//paramaters determined by testing:
const int left = 28;
const int right = 288;
const int top = 0;
const int bottom = 192;

//ball color signatures as assigned:
const int blueSig = 2; //blue ball signature number
const int greenSig = 1; //green ball signature number
const int otherSig = 3; //other color ball signature number

boolean stationary; //keep track of stationary/moving status

void setup() {
  Serial.begin(9600); //initialize serial monitor at 9600 baud rate
  Serial.print("Starting...\n"); //print so user knows state of pixy
  pixy.init(); //required to initialize pixy
}

void loop() {
  uint16_t blocks; //variable to hold # of detected objects in view with known signatures
  blocks = pixy.getBlocks(); //grab blocks
  if (blocks) {
    countDropped(); //determine how many of each color ball were dropped
    stationary = false;
    while (stationary == false) { //wait until all balls are stationary
      checkStationary(); //determine if all balls are stationary
    }
    //determine how many of each color ball are in the arena and the distance to each ball:
    countArena();
  }
}

void countDropped() {
  String title, totNum, blue, bNum, green, gNum, other, oNum; //strings for formatting output
  uint16_t totDropped[5] = {0};
  int blueDropped[5] = {0};
  int greenDropped[5] = {0};
  int otherDropped[5] = {0};

  for (int i = 0; i < 5; i++) {
    while (totDropped[i] == 0) {
      totDropped[i] = pixy.getBlocks();
    }
    //determine how many of each color were dropped:
    for (int j = 0; j < totDropped[i]; j++) {
      if (pixy.blocks[j].signature == blueSig) {
        blueDropped[i]++;
      } else if (pixy.blocks[j].signature == greenSig) {
        greenDropped[i]++;
      } else if (pixy.blocks[j].signature == otherSig) {
        otherDropped[i]++;
      }
    }
    delay(500);
  }

  int m = getIndexOfMaxValue(totDropped, 5); //index of array element with max value

  //format string and print:
  title = "Dropped:\nTotal=";
  totNum = title + totDropped[m];
  blue = totNum + "\nBlue=";
  bNum = blue + blueDropped[m];
  green = bNum + "\nGreen=";
  gNum = green + greenDropped[m];
  other = gNum + "\nOther=";
  oNum = other + otherDropped[m];
  Serial.println(oNum);
}

int getIndexOfMaxValue(uint16_t* array, int aSize) {
  int maxIndex = 0;
  int maxVal = array[maxIndex];
  for (int i = 1; i < aSize; i++) {
    if (maxVal < array[i]) {
      maxVal = array[i];
      maxIndex = i;
    }
  }
  return maxIndex;
}

//determine if all balls are stationary:
void checkStationary() {
  //old and current position values for comparison:
  int oldX;
  int oldY;
  int currentX;
  int currentY;
  uint16_t blocks = 0;
  while (blocks == 0) {
    blocks = pixy.getBlocks();
  }
  for (int i = 0; i < blocks; i++) {
    oldX = pixy.blocks[i].x;
    oldY = pixy.blocks[i].y;
    delay(1000);
    while (stationary == false) {
      currentX = pixy.blocks[i].x;
      currentY = pixy.blocks[i].y;
      if (currentX == oldX && currentY == oldY) {
        stationary = true;
      } else {
        oldX = currentX;
        oldY = currentY;
        delay(1000);
      }
    }
  }
}

//determine how many of each color ball are in the arena:
void countArena() {
  String title, totNum, blue, bNum, green, gNum, other, oNum, color, ball, dist, sentence; //strings for formatting output
  int arenaCount = 0;
  int blueCount = 0;
  int greenCount = 0;
  int otherCount = 0;

  uint16_t blocks = 0;
  while (blocks == 0) {
    blocks = pixy.getBlocks();
  }
  for (int i = 0; i < blocks; i++) { //check each block
    if (pixy.blocks[i].x > left + pixy.blocks[i].width / 2 &&
        pixy.blocks[i].x < right - pixy.blocks[i].width / 2 &&
        pixy.blocks[i].y < bottom + pixy.blocks[i].height / 2 &&
        pixy.blocks[i].y > top + pixy.blocks[i].height / 2) {
      arenaCount++;
      if (pixy.blocks[i].signature == blueSig) {
        blueCount++;
        color = "Blue ";
      } else if (pixy.blocks[i].signature == greenSig) {
        greenCount++;
        color = "Green ";
      } else if (pixy.blocks[i].signature == otherSig) {
        otherCount++;
        color = "Other ";
      }
      float distance = calcDist(i); //calculate distance to block i
      //format string and print distance to block i:
      ball = color + "ball ";
      dist = ball + distance;
      sentence = dist + " cm away.";
      Serial.println(sentence);
    }
  }
  //format string; print total and how many of each color ball in arena:
  title = "Arena:\nTotal=";
  totNum = title + arenaCount;
  blue = totNum + "\nBlue=";
  bNum = blue + blueCount;
  green = bNum + "\nGreen=";
  gNum = green + greenCount;
  other = gNum + "\nOther=";
  oNum = other + otherCount;
  Serial.println(oNum);

  while (true); //do nothing while true so program essentially stops here
}

//calculate distance to ball:
float calcDist(int num) {
  //X Coordinate Boundaries
  //left side: edge to 108
  //between center & left side: 109 to 150
  //center: 151 to 183
  //between center & right side: 184 to 225
  //right side: 226 to edge
  
  int x = pixy.blocks[num].x;
  int w = pixy.blocks[num].width; //get ball width
  int h = pixy.blocks[num].height; //get ball width
  float A = (float)w * (float)h; //area of block rectangle
  float dist = 139.78 * exp(-0.057 * (float)w); //overall exponential width relation
  //float dist;
//  if (x < 109) {
//    //use side relationship
//    //dist = 105.55 * exp(-0.003*A); //area relation
//  } else if (x > 108 && x < 151) {
//    //use between relationship
//    //dist = 95.059 * exp(-0.002*A); //area relation
//  } else if (x > 150 && x < 184) {
//    //use center relationship
//    //dist = 92.735 * exp(-0.002*A); //area relation
//  } else if (x > 183 && x < 226) {
//    //use between relationship
//    //dist = 95.059 * exp(-0.002*A); //area relation
//  } else if (x > 225) {
//    //use side relationship
//    //dist = 105.55 * exp(-0.003*A); //area relation
//  }
  
  //float dist = 87 * exp(-0.001 * A); //relationship determined by calibration testing

  return dist;
}

