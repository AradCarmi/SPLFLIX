#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>


class Session;
class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual ~Watchable();
    virtual std::string toString() const = 0;
    virtual Watchable* getNextWatchable(Session&) const = 0;
    //mine
    Watchable(Watchable &&other);
    virtual Watchable* clone() const=0;
    Watchable(const Watchable& watchable);
    const long getId() const;
    int getLength() const;
    const std::vector<std::string>& getTags() const;
    virtual void setNextEpisodeId(int id) =0;
    //
private:
    const long id;
    int length;
    std::vector<std::string> tags;
};
class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    //mine
    Movie(const Movie& movie);
    Movie(Movie &&other);
    ~Movie();
    virtual Movie* clone() const;
    std::string getName();
    virtual void setNextEpisodeId(int id);
    //
private:
    std::string name;
};
class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    //mine
    Episode(const Episode& episode);
    ~Episode();
    Episode(Episode &&other);
    std::string getSeriesName() const;
    int getSeason() const;
    int getEpisode() const;
    long getNextEpisodeId() const;
    virtual void setNextEpisodeId(int id);
    Episode* clone() const;
    //
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;
};

#endif
