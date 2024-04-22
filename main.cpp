#include <iostream>
#include <math.h>
#include <cstdlib>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Playing field dimensions
#define MIN_X 0
#define MAX_X 25
#define MIN_Y -6
#define MAX_Y 6
#define DOOR_TOP 3
#define DOOR_BOTTOM -3

// Relative speed of all characters
#define PLAYER_SPEED 1.5
#define DOG_SPEED 1.0
#define CAT_SPEED 0.75

// Define range at which each can be caught
#define DOG_RANGE 2.0
#define CAT_RANGE 1.0

// Set true if you want to see position and velocity of each character (useful for debugging)
#define debug false

class Player;
class Dog;
class Cat;

// Base class of all characters in game. You can use these methods to get information
// about this or other characters in game.
class Entity
{
public:
    Entity(float, float, float, float);

    // If moving true, move character in direction of angle (with monmentum).
    // Your steering methods will call this.
    virtual void move(bool moving, double angle);

    // Getters for current position and velocity of a character
    double getX();
    double getY();
    double getVelX();
    double getVelY();

    // Returns 1 if still playing, 0 if caught, 2 if escaped
    int getStatus();

    // Useful utility methods to find distance from this character to some given point,
    // and angle from this character to that point (really useful for steering).
    double getDistance(double, double);
    double getAngle(double, double);

    // Other methods used to implement game
    void setStatus(int);
    int getIntX();
    int getIntY();
    void printData();
    bool adjacent(Entity, float);

protected:
    double x, y, velX, velY;
    double speed;
    int status;
    double range;
};

class Player : public Entity
{
public:
    Player(double, double, double);
    void move(bool, double);

private:
};

class Dog : public Entity
{
public:
    Dog(double, double, double, double);
    void steer(Player, Cat);
    void check(Player);
};

class Cat : public Entity
{
public:
    Cat(double, double, double, double);
    void steer(Player, Dog);
    void check(Player);
};

Entity::Entity(float sx, float sy, float sp, float r)
{
    x = sx;
    y = sy;
    speed = sp;
    range = r;
    velX = 0;
    velY = 0;
    status = 1;
}

void Entity::move(bool moving, double angle)
{
    if (status != 1)
        return;
    if (moving)
    {
        double newX = cos(angle); // Get x, y components of angle
        double newY = sin(angle);
        velX += newX; // Add to current velocities
        velY += newY;
        double magnitude = sqrt(velX * velX + velY * velY); // normalize to 1
        if (magnitude > speed)
        {
            velX *= speed / magnitude;
            velY *= speed / magnitude;
        }
    }
    x += velX; // Move by the new velocities
    y += velY;
}

double Entity::getX()
{
    return x;
}

double Entity::getY()
{
    return y;
}

double Entity::getVelX()
{
    return velX;
}

double Entity::getVelY()
{
    return velY;
}

int Entity::getStatus()
{
    return status;
}

void Entity::setStatus(int s)
{
    status = s;
}

int Entity::getIntX()
{
    return (int)(x * 2.0);
}

int Entity::getIntY()
{
    return (int)y;
}

void Entity::printData()
{
    cout << " position (" << x << ", " << y << ") vel(" << velX << ", " << velY << ") status " << status << "\n";
}

bool Entity::adjacent(Entity e, float range)
{
    return (getDistance(e.getX(), e.getY()) <= range);
}

Player::Player(double x, double y, double s) : Entity(x, y, s, 1.0)
{
}

void Player::move(bool moving, double angle)
{
    angle *= 3.14159 / 180.0; // Convert to radians
    Entity::move(moving, angle);
    if (getX() < MIN_X || getY() < MIN_Y || getY() > MAX_Y || getX() > MAX_X)
    {
        cout << "You are caught in the fence! You lose!\n";
        setStatus(0);
    }
}

Dog::Dog(double x, double y, double s, double r) : Entity(x, y, s, r)
{
}

void Dog::check(Player player)
{
    if (adjacent(player, range))
    {
        cout << "You have caught the dog!\n";
        setStatus(0);
    }
    if (getY() >= DOOR_BOTTOM && getY() <= DOOR_TOP && getX() >= MAX_X)
    {
        cout << "The dog has escaped! You lose!\n";
        setStatus(2);
    }
    else if (getX() < MIN_X || getY() < MIN_Y || getY() > MAX_Y || getX() > MAX_X)
    {
        cout << "The dog is caught in the fence!\n";
        setStatus(0);
    }
}
Cat::Cat(double x, double y, double s, double r) : Entity(x, y, s, r)
{
}

void Cat::check(Player player)
{
    if (adjacent(player, range))
    {
        cout << "You have caught the cat!\n";
        setStatus(0);
    }
    if (getY() >= DOOR_BOTTOM && getY() <= DOOR_TOP && getX() >= MAX_X)
    {
        cout << "The cat has escaped! You lose!\n";
        setStatus(2);
    }
    else if (getX() < MIN_X || getY() < MIN_Y || getY() > MAX_Y || getX() > MAX_X)
    {
        cout << "The cat is caught in the fence!\n";
        setStatus(0);
    }
}

void display(Player player, Dog dog, Cat cat)
{
    cout << "+--------------------------------------------------+\n";
    for (int y = MAX_Y; y >= MIN_Y; y--)
    {
        cout << "|";
        for (int x = 1; x <= 2 * MAX_X; x++)
        {
            if (player.getIntX() == x && player.getIntY() == y && player.getStatus() == 1)
            {
                cout << "P";
            }
            else if (dog.getIntX() == x && dog.getIntY() == y && dog.getStatus() == 1)
            {
                cout << "D";
            }
            else if (cat.getIntX() == x && cat.getIntY() == y && cat.getStatus() == 1)
            {
                cout << "C";
            }
            else
            {
                cout << " ";
            }
        }
        if (y < DOOR_BOTTOM || y > DOOR_TOP)
        {
            cout << "|\n";
        }
        else
        {
            cout << "\n";
        }
    }
    cout << "+--------------------------------------------------+\n";
}

double getInput()
{
    cout << "Direction to move: 1 2 3\n";
    cout << "                   4 0 5\n";
    cout << "                   6 7 8: ";
    int direction;
    cin >> direction;
    switch (direction)
    {
    case 0:
        return -1.0;
        break;
    case 1:
        return 135.0;
        break;
    case 2:
        return 90.0;
        break;
    case 3:
        return 45.0;
        break;
    case 4:
        return 180.0;
        break;
    case 5:
        return 0.0;
        break;
    case 6:
        return 225.0;
        break;
    case 7:
        return 270.0;
        break;
    case 8:
        return 315.0;
        break;
    }
    return 0;
}

int main()
{
    Player player(20, 0, PLAYER_SPEED);
    Dog dog(5, 1, DOG_SPEED, DOG_RANGE);
    Cat cat(5, -1, CAT_SPEED, CAT_RANGE);

    double angle;
    display(player, dog, cat);
    bool playing = true;
    while (playing)
    {
        if (dog.getStatus() == 1)
        {
            dog.steer(player, cat);
        }
        if (cat.getStatus() == 1)
        {
            cat.steer(player, dog);
        }

        angle = getInput();
        if (angle < 0)
        {
            player.move(false, 0.0);
        }
        else
        {
            player.move(true, angle);
        }
        display(player, dog, cat);
        if (dog.getStatus() == 1)
        {
            dog.check(player);
        }
        if (cat.getStatus() == 1)
        {
            cat.check(player);
        }

        if (player.getStatus() == 0)
        {
            playing = false;
        }
        if (cat.getStatus() == 2 || dog.getStatus() == 2)
        {
            playing = false;
        }
        if (cat.getStatus() == 0 && dog.getStatus() == 0)
        {
            cout << "You have caught both the cat and the dog! You win!!\n";
            playing = false;
        }

        if (debug)
        {
            cout << "Player ";
            player.printData();
            cout << "Dog ";
            dog.printData();
            cout << "Cat ";
            cat.printData();
        }
    }
    return 0;
}

/************************** Useful support methods ********************************/

// This returns the distance from this object to the location (tx, ty).
double Entity::getDistance(double tx, double ty)
{
    double delX = x - tx;
    double delY = y - ty;
    return sqrt(delX * delX + delY * delY);
}

// This returns the angle at which this character would have to move in order to reach the location (tx, ty)
double Entity::getAngle(double tx, double ty)
{
    double delX = tx - x;
    double delY = ty - y;
    return atan2(delY, delX);
}

/************************** Code to modify is below *******************************/

// This method determines how the cat is moved each turn.
// It must call the move method at the end with two parameters:
// -- Whether the cat should move at all (true or false)
// -- If so, what direction the cat should move in.
//    This must be a value in radians (that is, between 0 and 2*pi)

//****************************************************************************************************************
// Both animals somewhat work togther by the cat being harder to catch and the dog going for the exit more urgently.
// If the player goes for the cat first, the dog will most likely escape and the player will lose.
// Both animals are easier to catch from behind because they cannot "see" the player.
//****************************************************************************************************************

// This method defines the evasion and movement strategy for the cat character.
// The cat uses a combination of predictive evasion, random movement, and boundary avoidance strategies.
// The goal is to evade the player intelligently while trying to escape through the doorway.
void Cat::steer(Player player, Dog dog)
{
    // Obtain player's current position and velocity to predict future position for evasion.
    double playerX = player.getX();
    double playerY = player.getY();
    double playerVelX = player.getVelX();
    double playerVelY = player.getVelY();

    // Increase the prediction time based on the player's speed and the cat's speed
    double predictionTime = 2.0; // Adjust this value to make evasion more effective
    double futurePlayerX = playerX + playerVelX * predictionTime;
    double futurePlayerY = playerY + playerVelY * predictionTime;

    // Calculate the angle and distance to the door to determine movement priority.
    double doorX = 25, doorY = 0;
    double angleToDoor = getAngle(doorX, doorY);
    double distanceToDoor = getDistance(doorX, doorY);

    double angle;

    // Prioritize escaping towards the door when in proximity and alignment
    if (distanceToDoor < 5 && y >= DOOR_BOTTOM && y <= DOOR_TOP)
    {
        angle = angleToDoor;
    }
    else
    {
        // Evade by moving away from the player's predicted future position
        double distanceToFuturePlayer = getDistance(futurePlayerX, futurePlayerY);
        if (distanceToFuturePlayer < 10) // Increased range for responsive evasion
        {
            double angleToFuturePlayer = getAngle(futurePlayerX, futurePlayerY);
            angle = angleToFuturePlayer + M_PI; // Move in the opposite direction
        }
        else
        {
            // Default behavior with randomness when not evading
            angle = angleToDoor + (rand() % 2 ? -0.2 : 0.2);
        }
    }

    // Boundary avoidance without using an additional function
    double boundaryMargin = 3;
    double tempX = x + cos(angle) * 5;
    double tempY = y + sin(angle) * 5;
    if (tempX < MIN_X + boundaryMargin || tempX > MAX_X - boundaryMargin)
    {
        angle += M_PI / 2; // Adjust to avoid horizontal boundaries
    }
    if (tempY < MIN_Y + boundaryMargin || tempY > MAX_Y - boundaryMargin)
    {
        angle += M_PI / 2; // Adjust to avoid vertical boundaries
    }

    // Direct adjustment if next to a boundary
    if (x < MIN_X + boundaryMargin || x > MAX_X - boundaryMargin || y < MIN_Y + boundaryMargin || y > MAX_Y - boundaryMargin)
    {
        if (x < MIN_X + boundaryMargin)
        {
            angle = 0; // Move right
        }
        else if (x > MAX_X - boundaryMargin)
        {
            angle = M_PI; // Move left
        }

        if (y < MIN_Y + boundaryMargin)
        {
            angle = M_PI / 2; // Move up
        }
        else if (y > MAX_Y - boundaryMargin)
        {
            angle = -M_PI / 2; // Move down
        }
    }

    move(true, angle);
}

// This method outlines the dog's strategy focusing on escaping while also `avoiding the player.
// The dog uses a more aggressive escape strategy compared to the cat, with a mix of evasion, randomization, and boundary awareness.
void Dog::steer(Player player, Cat cat)
{
    // Gather player's current position to calculate evasion and escape angles.
    double playerX = player.getX();
    double playerY = player.getY();
    double distanceToPlayer = getDistance(playerX, playerY);
    double angleToPlayer = getAngle(playerX, playerY);
    double angle;

    // Calculate angle towards the door
    double doorX = 25, doorY = 0; // Coordinates of the door
    double angleToDoor = getAngle(doorX, doorY);
    double distanceToDoor = getDistance(doorX, doorY);

    if (distanceToDoor < 5)
    {
        angle = angleToDoor; // Directly move towards the door
    }
    else if (distanceToPlayer < 4)
    {
        // Move directly away from the player if very close
        angle = angleToPlayer + M_PI;
    }
    else
    {
        // Balance between moving towards the exit and avoiding the player
        if (distanceToPlayer < 6)
        {
            // Mix the angle to move slightly away from the player and towards the door
            angle = (3 * angleToDoor + angleToPlayer + M_PI) / 4;
        }
        else
        {
            // Prioritize the door if the player is far enough
            angle = angleToDoor;
        }
    }

    // Introduce a small random component to the angle
    double randomComponent = (rand() % 20 - 10) * M_PI / 180; // Random angle between -10 and 10 degrees
    angle += randomComponent;

    // Boundary sensitivity logic
    double boundaryMargin = 4;
    bool nearBoundary = false;
    double adjustAngle = 0;

    if (x <= MIN_X + boundaryMargin || x >= MAX_X - boundaryMargin)
    {
        nearBoundary = true;
        adjustAngle = (y < doorY) ? M_PI / 2 : -M_PI / 2;
    }
    if (y <= MIN_Y + boundaryMargin || y >= MAX_Y - boundaryMargin)
    {
        nearBoundary = true;
        adjustAngle = (x < doorX) ? 0 : M_PI;
    }

    if (nearBoundary && distanceToDoor >= 5)
    {
        angle = adjustAngle;
    }

    move(true, angle);
}
