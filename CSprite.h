//sprite actions
enum SPRITEACTION {SA_NONE, SA_KILL, SA_ADDSPRITE};
//actions in case of sprite reaching a bound.
enum BOUNDSACTION {BA_STOP, BA_WRAP, BA_BOUNCE, BA_DIE};

class CSprite
{
protected:
  // Member Variables
  sf::Sprite    psprite;
  std::string    name;
  sf::Vector2f  velocity;
  int           ZOrder;
  sf::FloatRect rcCollision;
  sf::FloatRect rcBounds;
  BOUNDSACTION  BoundsAction;
  bool          Hidden;
  bool          Dying;
  bool          oneCycle;

  // animation variables.
  int numFrames;
  int curFrame;
  int frameDelay;
  int frameTrigger;

  // helper method
  void         UpdateFrame();
  virtual void CalcCollisionRect();

public:
  // Constructor(s)/Destructor
  CSprite(const std::string &texture);
  CSprite(const std::string &texture, sf::FloatRect &Bounds,
    BOUNDSACTION baBoundsAction = BA_STOP);
  CSprite(const std::string &texture, sf::Vector2f pPosition, sf::Vector2f pVelocity, int iZOrder,
    sf::FloatRect &Bounds, BOUNDSACTION baBoundsAction = BA_STOP);
  virtual ~CSprite();

  // General Methods
  virtual SPRITEACTION  Update(sf::Time delta);
  virtual CSprite*      AddSprite();
  void          Draw(sf::RenderWindow &window);
  bool          IsPointInside(float x, float y);
  bool          TestCollision(CSprite* pTestSprite);
  void          Kill() {  Dying = true; };

  // Accessor Methods
  sf::FloatRect   GetPosition() { return psprite.getGlobalBounds(); };
  void    SetPosition(float x, float y);
  void    SetPosition(sf::Vector2f vPosition);
  void    SetPosition(sf::FloatRect rcPosition);
  void    OffsetPosition(float x, float y);
  sf::FloatRect  GetCollision() { return rcCollision; };
  sf::Vector2f   GetVelocity() { return velocity; };
  void    SetVelocity(float x, float y);
  void    SetVelocity(sf::Vector2f pvelocity);
  bool    GetZOrder()               { return ZOrder; };
  void    SetZOrder(int iZOrder)    { ZOrder = iZOrder; };
  void    SetBounds(sf::FloatRect prcBounds) { rcBounds = prcBounds; };
  void    SetBoundsAction(BOUNDSACTION ba) { BoundsAction = ba; };
  bool    IsHidden()                { return Hidden; };
  void    SetHidden(bool bHidden)   { Hidden = bHidden; };
  int     GetWidth()                { return psprite.getTexture()->getSize().x / numFrames; };
  int     GetHeight()               { return psprite.getTexture()->getSize().y; };
  std::string GetName() { return name; };
  void SetName(std::string str) { name = str; };
  void setNumFrames(int inumFrames, bool boneCycle = false);
  void setFrameDelay(int iframeDelay) { frameDelay = iframeDelay; };
};

//-----------------------------------------------------------------
// Sprite Inline Helper Methods
//-----------------------------------------------------------------
inline void CSprite::UpdateFrame()
{
    if(( frameDelay >= 0 ) && (--frameTrigger <= 0) )
    {
        // reset the frame trigger
        frameTrigger = frameDelay;

        // increment the frame
        if(++curFrame >= numFrames)
        {
            //if it's a one-cycle frame animation, kill the sprite
            if( oneCycle )
                Dying = true;
            else
                curFrame = 0;
        }

        //update the rect
        sf::IntRect rect = psprite.getTextureRect();
        rect.left = rect.width * curFrame;
        psprite.setTextureRect(rect);
    }
}

inline void CSprite::CalcCollisionRect()
{
  int iXShrink = psprite.getGlobalBounds().width / 12;
  int iYShrink = psprite.getGlobalBounds().height / 12;
  rcCollision = psprite.getGlobalBounds();
  rcCollision.left += iXShrink;
  rcCollision.top += iYShrink;
  rcCollision.width -= iXShrink * 2;
  rcCollision.height -= iYShrink * 2;
}

//-----------------------------------------------------------------
// Sprite Inline General Methods
//-----------------------------------------------------------------
inline bool CSprite::TestCollision(CSprite* pTestSprite)
{
    sf::FloatRect rcTest = pTestSprite->GetCollision();
    return rcCollision.left <= (rcTest.left + rcTest.width) &&
            rcTest.left <= (rcCollision.left + rcCollision.width) &&
            rcCollision.top <= (rcTest.top + rcTest.height) &&
            rcTest.top <= (rcCollision.top + rcCollision.height);
}

inline bool CSprite::IsPointInside(float x, float y)
{
  return psprite.getGlobalBounds().contains(x,y);
}

//-----------------------------------------------------------------
// Sprite Inline Accessor Methods
//-----------------------------------------------------------------

//----------
// animation
//----------
inline void CSprite::setNumFrames(int inumFrames, bool boneCycle)
{
    //set the number of frames
    numFrames = inumFrames;
    oneCycle = boneCycle;

    //recalculate the position
    sf::IntRect rect = psprite.getTextureRect();
    rect.width = rect.width / numFrames;
    psprite.setTextureRect(rect);
}

inline void CSprite::SetPosition(float x, float y)
{
    psprite.setPosition(x, y);
    CalcCollisionRect();
}

inline void CSprite::SetPosition(sf::Vector2f vPosition)
{
    psprite.setPosition(vPosition.x, vPosition.y);
    CalcCollisionRect();
}

inline void CSprite::SetPosition(sf::FloatRect rcPosition)
{
    psprite.setPosition(rcPosition.left, rcPosition.top);
    CalcCollisionRect();
}

inline void CSprite::OffsetPosition(float x, float y)
{
    psprite.setPosition( psprite.getGlobalBounds().left + x, psprite.getGlobalBounds().top + y);
    CalcCollisionRect();
}

inline void CSprite::SetVelocity(float x, float y)
{
  velocity.x = x;
  velocity.y = y;
}

inline void CSprite::SetVelocity(sf::Vector2f pvelocity)
{
  velocity.x = pvelocity.x;
  velocity.y = pvelocity.y;
}

//////////////////////////////////////////////////////////////

CSprite::CSprite(const std::string &texture)
{
  // Initialize the member variables
  psprite.setTexture(mTextures[texture]);
  psprite.setPosition(0,0);

  CalcCollisionRect();
  velocity.x = velocity.y = 0;
  ZOrder = 0;
  rcBounds = sf::FloatRect(0,0,640,480); //default values

  numFrames = 1;
  curFrame = frameDelay = frameTrigger = 0;

  BoundsAction = BA_STOP;
  Hidden = false;
  Dying = false;
  oneCycle = false;
}

CSprite::CSprite(const std::string &texture, sf::FloatRect &prcBounds, BOUNDSACTION baBoundsAction)
{
  // Initialize the member variables
  psprite.setTexture(mTextures[texture]);
  psprite.setPosition(0,0);

  CalcCollisionRect();
  velocity.x = velocity.y = 0;
  ZOrder = 0;
  rcBounds = prcBounds;

  numFrames = 1;
  curFrame = frameDelay = frameTrigger = 0;

  BoundsAction = baBoundsAction;
  Hidden = false;
  Dying = false;
  oneCycle = false;
}

CSprite::CSprite(const std::string &texture, sf::Vector2f ptPosition, sf::Vector2f ptVelocity, int iZOrder,
    sf::FloatRect &prcBounds, BOUNDSACTION baBoundsAction)
{
  // Initialize the member variables
  psprite.setTexture(mTextures[texture]);
  psprite.setPosition(ptPosition.x, ptPosition.y);

  CalcCollisionRect();
  velocity = ptVelocity;
  ZOrder = iZOrder;
  rcBounds = prcBounds;

  numFrames = 1;
  curFrame = frameDelay = frameTrigger = 0;

  BoundsAction = baBoundsAction;
  Hidden = false;
  Dying = false;
  oneCycle = false;
}

CSprite::~CSprite()
{
}

//-----------------------------------------------------------------
// Sprite General Methods
//-----------------------------------------------------------------
SPRITEACTION CSprite::Update(sf::Time delta)
{
    //see if the sprite needs to be killed
    if( Dying ) return SA_KILL;

    //update the frame
    UpdateFrame();

  // Update the position
  sf::Vector2f ptNewPosition, ptSpriteSize, ptBoundsSize;
  ptNewPosition.x = psprite.getGlobalBounds().left + velocity.x * delta.asSeconds();
  ptNewPosition.y = psprite.getGlobalBounds().top  + velocity.y * delta.asSeconds();
  ptSpriteSize.x = psprite.getGlobalBounds().width;
  ptSpriteSize.y = psprite.getGlobalBounds().height;
  ptBoundsSize.x = rcBounds.width;
  ptBoundsSize.y = rcBounds.height;

  // Check the bounds
  // Wrap?
  if (BoundsAction == BA_WRAP)
  {
    if ((ptNewPosition.x + ptSpriteSize.x) < rcBounds.left)
      ptNewPosition.x = rcBounds.left + rcBounds.width;
    else if (ptNewPosition.x > rcBounds.left + rcBounds.width)
      ptNewPosition.x = rcBounds.left - ptSpriteSize.x;
    if ((ptNewPosition.y + ptSpriteSize.y) < rcBounds.top)
      ptNewPosition.y = rcBounds.top + rcBounds.height;
    else if (ptNewPosition.y > rcBounds.top + rcBounds.height)
      ptNewPosition.y = rcBounds.top - ptSpriteSize.y;
  }
  // Bounce?
  else if (BoundsAction == BA_BOUNCE)
  {
    bool bBounce = false;
    sf::Vector2f ptNewVelocity = velocity;
    if (ptNewPosition.x < rcBounds.left)
    {
      bBounce = true;
      ptNewPosition.x = rcBounds.left;
      ptNewVelocity.x = -ptNewVelocity.x;
    }
    else if ((ptNewPosition.x + ptSpriteSize.x) > rcBounds.left + rcBounds.width)
    {
      bBounce = true;
      ptNewPosition.x = rcBounds.left + rcBounds.width - ptSpriteSize.x;
      ptNewVelocity.x = -ptNewVelocity.x;
    }
    if (ptNewPosition.y < rcBounds.top)
    {
      bBounce = true;
      ptNewPosition.y = rcBounds.top;
      ptNewVelocity.y = -ptNewVelocity.y;
    }
    else if ((ptNewPosition.y + ptSpriteSize.y) > rcBounds.top + rcBounds.height)
    {
      bBounce = true;
      ptNewPosition.y = rcBounds.top + rcBounds.height - ptSpriteSize.y;
      ptNewVelocity.y = -ptNewVelocity.y;
    }
    if (bBounce)
      SetVelocity(ptNewVelocity);
  }
  // Die?
  else if (BoundsAction == BA_DIE)
  {
      if((ptNewPosition.x + ptSpriteSize.x) < rcBounds.left ||
         ptNewPosition.x > (rcBounds.left + rcBounds.width) ||
         (ptNewPosition.y + ptSpriteSize.y) < rcBounds.top  ||
         ptNewPosition.y > (rcBounds.top + rcBounds.height))
            return SA_KILL;
  }
  // Stop (default)
  else
  {
    if (ptNewPosition.x  < rcBounds.left ||
      ptNewPosition.x > (rcBounds.left + rcBounds.width - ptSpriteSize.x))
    {
      ptNewPosition.x = std::max(rcBounds.left, std::min(ptNewPosition.x,
        rcBounds.left + rcBounds.width - ptSpriteSize.x));
      SetVelocity(0, 0);
    }
    if (ptNewPosition.y  < rcBounds.top ||
      ptNewPosition.y > (rcBounds.top + rcBounds.height - ptSpriteSize.y))
    {
      ptNewPosition.y = std::max(rcBounds.top, std::min(ptNewPosition.y,
        rcBounds.top + rcBounds.height - ptSpriteSize.y));
      SetVelocity(0, 0);
    }
  }
  SetPosition(ptNewPosition);

  return SA_NONE;
}

CSprite* CSprite::AddSprite()
{
    return nullptr;
}

void CSprite::Draw(sf::RenderWindow &window)
{
  // Draw the sprite if it isn't hidden
  if (!Hidden)
    window.draw(psprite);
}


