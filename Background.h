class Background{
protected:
    int width, height;
    bool solid = false;
    sf::Color color;
    sf::Texture texture;

public:
    Background(int pwidth, int pheight, sf::Color pcolor);
    Background(sf::Texture ptexture);
    virtual ~Background();

    //general methods
    virtual void Update();
    virtual void Draw(sf::RenderWindow &window);

    //accessor methods
    int getWidth() { return width; };
    int getHeight() { return height; };
};

// horizontal scrolling background
class ScrollingBackground : Background{
public:
    int speed;
    sf::IntRect bgRect;

    ScrollingBackground(const std::string &stexture, int width, int height, float fspeed);
    virtual ~ScrollingBackground();

    //general methods
    virtual void Update();
    virtual void Draw(sf::RenderWindow &window);
};

// starry background class
class StarryBackground : Background{
protected:
    int numStars;
    int twinkleDelay;
    sf::Vector2i ptStars[100];
    sf::Color starColors[100];

public:
    StarryBackground(int pwidth, int pheight, int pnumStars = 100, int ptwinkleDelay = 50);
    virtual ~StarryBackground();

    //general methods
    virtual void Update();
    virtual void Draw(sf::RenderWindow &window);
};

////////////////////////////////////////////////////////////////////////////////

//constructors
Background::Background(int pwidth, int pheight, sf::Color pcolor)
{
    width = pwidth;
    height = pheight;
    color = pcolor;
    solid = true;
}

Background::Background(sf::Texture ptexture)
{
    color = sf::Color::Black;
    texture = ptexture;
    solid = false;
    width = ptexture.getSize().x;
    height = ptexture.getSize().y;
}

Background::~Background()
{
}

//general methods
void Background::Update()
{
    //do nothing since the basic background is not animated
}

void Background::Draw(sf::RenderWindow &window)
{
    //draw the background if there is one
    if( !solid )
    {
        sf::Sprite sp;
        sp.setTexture(texture);
        sp.setPosition(0,0);
        window.draw(sp);
    }
    else
    {
        window.clear(color);
    }
}

//starrybackground constructor
StarryBackground::StarryBackground(int pwidth, int pheight, int pnumStars, int ptwinkleDelay) : Background(pwidth, pheight, sf::Color::Black)
{
    numStars = std::min(pnumStars, 100);
    twinkleDelay = ptwinkleDelay;

    //create the stars
    for (int i=0;i<numStars;i++)
    {
        ptStars[i].x = rnd.getRndInt(0,width);
        ptStars[i].y = rnd.getRndInt(0,height);
        sf::Color c(128,128,128,255);
        starColors[i] = c;
    }
}

StarryBackground::~StarryBackground()
{
}

//StarryBackground general methods
void StarryBackground::Update()
{
    //randomly change the shade of the stars so that they twinkle
    int rgb;
    for(int i=0;i<numStars;i++)
    {
        if( rnd.getRndInt(0,twinkleDelay) == 0 )
        {
            rgb = rnd.getRndInt(0,255);
            sf::Color c(rgb,rgb,rgb,255);
            starColors[i] = c;
        }
    }
}

void StarryBackground::Draw(sf::RenderWindow &window)
{
    //draw the solid black background
    window.clear(sf::Color::Black);

    // Create a image filled with black color
    sf::Image image;
    image.create(width, height, sf::Color::Black);

    //Draw the stars in the image
    for(int i=0;i<numStars;i++)
    {
        image.setPixel(ptStars[i].x, ptStars[i].y, starColors[i]);
    }

    //create a texture with the image
    sf::Texture tx;
    tx.create(width,height); //texture needs to be created first.
    tx.update(image);

    //display it through sprite
    sf::Sprite sp;
    sp.setTexture(tx);
    sp.setPosition(0,0);
    window.draw(sp);
}

ScrollingBackground::ScrollingBackground(const std::string &stexture, int width, int height, float fspeed) : Background(mTextures[stexture])
{
    speed = fspeed;
    bgRect = sf::IntRect(0,0,width,height);
    texture.setRepeated(true);
}

ScrollingBackground::~ScrollingBackground()
{
}

void ScrollingBackground::Update()
{
    bgRect.left += speed;
    if( bgRect.left >= width ) bgRect.left = 0;
}

void ScrollingBackground::Draw(sf::RenderWindow &window)
{
    sf::Sprite background;
    background.setTexture(texture);
    background.setTextureRect(bgRect);
    background.setPosition(0,0);
    window.draw(background);
}

