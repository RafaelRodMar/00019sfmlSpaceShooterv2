//Textures
std::map<std::string, sf::Texture> mTextures;
//fonts
std::map<std::string, sf::Font> mFonts;
//Sound Buffers
std::map<std::string, sf::SoundBuffer> mSoundBuffers;
//Sounds
std::map<std::string, sf::Sound> mSounds;
//Music
std::map<std::string, sf::Music*> mMusic;

class Rnd{
public:
    std::mt19937 rng;

    Rnd()
    {
        std::mt19937 prng(std::chrono::steady_clock::now().time_since_epoch().count());
        rng = prng;
    }

    int getRndInt(int min, int max)
    {
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(rng);
    }

    double getRndDouble(double min, double max)
    {
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(rng);
    }
};

Rnd rnd;

void UpdateHiScores(std::vector<int> &v, int newscore)
{
    //new score to the end
    v.push_back(newscore);
    //sort
    sort(v.rbegin(), v.rend());
    //remove the last
    v.pop_back();
}

void ReadHiScores(std::vector<int> &v)
{
    std::ifstream in("hiscores.dat");
    if(in.good())
    {
        std::string str;
        getline(in,str);
        std::stringstream ss(str);
        int n;
        for(int i=0;i<5;i++)
        {
            ss >> n;
            v.push_back(n);
        }
        in.close();
    }
    else
    {
        //if file does not exist fill with 5 scores
        for(int i=0;i<5;i++)
        {
            v.push_back(0);
        }
    }
}

void WriteHiScores(std::vector<int> &v)
{
    std::ofstream out("hiscores.dat");
    for(int i=0;i<5;i++)
    {
        out << v[i] << " ";
    }
    out.close();
}
