#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
using namespace std;
using namespace sf;
int x = 5;
constexpr int windowWidth ( 800 ), windowHeight( 600 );
constexpr float ballRadius( 10.f ), ballVelocity( 6.f );
constexpr float paddleWidth( 100.f ), paddleHeight( 20.f ), paddleVelocity( 8.f );
constexpr float blockWidth( 60.f ), blockHeight( 20.f );
constexpr int countBlocksX( 11 ), countBlocksY( 6 );
constexpr int countBlocks2X(11), countBlocks2Y(3);
bool isPlaying = true;


struct Ball
{
        CircleShape shape;
        Vector2f velocity{ -ballVelocity, -ballVelocity };

        Ball(float mX, float mY)
        {
                shape.setPosition(mX, mY);
                shape.setRadius(ballRadius);
                shape.setFillColor(Color::Yellow);
                shape.setOrigin(ballRadius, ballRadius);
        }

        void update()
        {
                //Need to make the ball bounce of the window edges
                shape.move(velocity);
                //If it's leaving on the left edge, we set a positive horizontal value.
                if (left() < 0)
                        velocity.x = ballVelocity;
                //Same for the right
                else if (right() > windowWidth)
                        velocity.x = -ballVelocity;
                //Top
                if (top() < 0)
                        velocity.y = ballVelocity;
                //And bottom
                else if (bottom() > windowHeight)
                        velocity.y = -ballVelocity;
        
        }

        float x() { return shape.getPosition().x; }
        float y() { return shape.getPosition().y; }
        float left() { return x() - shape.getRadius(); }
        float right() { return x() + shape.getRadius(); }
        float top() { return y() - shape.getRadius(); }
        float bottom() { return y() + shape.getRadius(); }
};

//Create the Rectangle shape class for the brick
struct Rectangle
{
        RectangleShape shape;
        float x()               { return shape.getPosition().x; }
        float y()               { return shape.getPosition().y; }
        float left()    { return x() - shape.getSize().x / 2.f; }
        float right()   { return x() + shape.getSize().x / 2.f; }
        float top()             { return y() - shape.getSize().y / 2.f; }
        float bottom()  { return y() + shape.getSize().y / 2.f; }
};

//Class for the paddle
struct Paddle : public Rectangle
{
        //Create a variable for speed.
        Vector2f velocity;
        //Set the variables for the paddle rectangle shape.
        Paddle(float mX, float mY)
        {
                shape.setPosition(mX, mY);
                shape.setSize({ paddleWidth, paddleHeight });
                shape.setFillColor(Color::Red);
                shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
        }
        // Within the update function we check if the player is moving the paddle
        void update()
        {
                shape.move(velocity);
                //To ensure that the paddle stays inside the window we only change the Velocity when it's inside the boundaries
                //Making it impossible to move outside when the initial velocity is set to zero
                if (Keyboard::isKeyPressed(Keyboard::Key::Left) && left() > 0)
                velocity.x = -paddleVelocity;
                        
                else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && right() < windowWidth)
                        velocity.x = paddleVelocity;
                //If the player isn't pressing a buttom (legt/right) the velocity is set to zero.
                else
                        velocity.x = 0;
        }
};
//Another class for the bricks
struct Brick : public Rectangle
{
        bool destroyed{ false };

        Brick(float mX, float mY)
        {
                shape.setPosition(mX, mY);
                shape.setSize({ blockWidth, blockHeight });
                shape.setFillColor(Color::Black);
                shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
        }
};

//C++ Feature template allows us to create a generic funtion to check if two shapes are intersecting or colliding.
template <class T1, class T2>
bool isIntersecting(T1& mA, T2& mB)
{
        return mA.right() >= mB.left() && mA.left() <= mB.right() &&
                mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void collisionTest(Paddle& mPaddle, Ball& mBall)
{
        if (!isIntersecting(mPaddle, mBall)) return;

        mBall.velocity.y = -ballVelocity;
        if (mBall.x() < mPaddle.x())
                mBall.velocity.x = -ballVelocity;
        else
                mBall.velocity.x = ballVelocity;
}


void collisionTest(Brick& mBrick, Ball& mBall)
{
        if (!isIntersecting(mBrick, mBall)) return;
        mBrick.destroyed = true;

        float overlapLeft{ mBall.right() - mBrick.left() };
        float overlapRight{ mBrick.right() - mBall.left() };
        float overlapTop{ mBall.bottom() - mBrick.top() };
        float overlapBottom{ mBrick.bottom() - mBall.top() };

        bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
        bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

        float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
        float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

        if (abs(minOverlapX) < abs(minOverlapY))
                mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
        else
                mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
}
int main()
{
        
        //We render/create the window
        RenderWindow window(VideoMode(windowWidth, windowHeight ), "Breakout Game" );
        window.setFramerateLimit(60);
        
        Paddle paddle{ windowWidth / 2, windowHeight - 50 };
        int x = 5;
//Here we use an unconditiional goto statement to allow the user to restart the game.   
restart:
        //We reference the Ball, Paddle and Bricks
        Ball ball{ windowWidth / 2, windowHeight / 2 };
        vector<Brick> bricks;
        //vector<Brick2> bricks2;

        for (int iX{ 0 }; iX < countBlocksX; ++iX)
                for (int iY{ 0 }; iY < countBlocksY; ++iY)
                        bricks.emplace_back(
                                (iX + 1) * (blockWidth + 3) + 22, (iY + 2) * (blockHeight + 3));
        

        // Load the text font
        sf::Font font;
        if (!font.loadFromFile("arial.ttf"))
                return EXIT_FAILURE;

        // Initialize the pause message
        sf::Text loseGame;
        loseGame.setFont(font);
        loseGame.setCharacterSize(40);
        loseGame.setPosition(80.f, 150.f);
        loseGame.setColor(sf::Color::White);
        loseGame.setString("You lost, press 'Space' to play again.");

        // Load the sounds used in the game
        sf::SoundBuffer ballSoundBuffer;
        if (!ballSoundBuffer.loadFromFile("loseSound.wav"))
                return EXIT_FAILURE;
        sf::Sound loseGameSound(ballSoundBuffer);


        while (true)
        {
                window.clear(Color::Color(49, 79, 79));

                if (Keyboard::isKeyPressed(Keyboard::Key::Space))
                        goto restart;
                if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) 
                        break;
                
                ball.update();
                paddle.update();
                collisionTest(paddle, ball);
                for (auto& brick : bricks) collisionTest(brick, ball);
                bricks.erase(remove_if(begin(bricks), end(bricks),
                        [](const Brick& mBrick)
                {       
                        return mBrick.destroyed;
                }),
                        end(bricks));
                
                if (isPlaying)
                {
                        window.draw(ball.shape);
                        window.draw(paddle.shape);
                        for (auto& brick : bricks) window.draw(brick.shape);
                }
                
                else
                {
                        window.clear(Color::Black);
                        // Draw the pause message
                        loseGameSound.play();
                        window.draw(loseGame);
                        if (Keyboard::isKeyPressed(Keyboard::Key::Space))
                                goto restart;
                        
                }
                
                window.display();
                
        }

        return 0;
}
 Logged
GraphicsWhale
Full Member
***
 
Posts: 131
View Profile

Re: [Code Review] Simple Breakout/Arka
