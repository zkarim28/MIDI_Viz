//
//  Note.h
//  NewProject
//
//  Created by Zarif Karim on 1/24/24.
//

#ifndef Note_h
#define Note_h


#endif /* Note_h */

class Note
{
public:
    Note(int x, int y, int height, int velocity, bool whiteOrBlack);

    ~Note();

    int getX() {return x;};
    int getY() {return y;};
    int getHeight() {return height;};
    int getVelocity() {return velocity;};
    bool getWhiteOrBlack() {return whiteOrBlack;};
    
private:
    int x;
    int y;
    int height;
    int width;
    int velocity;
    bool whiteOrBlack; //white = false, black = true
};
