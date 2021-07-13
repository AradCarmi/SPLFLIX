#include "../include/Watchable.h"
#include "Session.h"
#include "User.h"
using namespace std;

//**************************************Watchable*******************************

Watchable::Watchable(long id, int length, const std::vector<string> &tags) : id(id), length(length), tags(tags)
{
}
Watchable::~Watchable()
{
}
Watchable::Watchable(const Watchable& watchable): id(watchable.id),length(watchable.length),tags(watchable.tags)
{
}
Watchable::Watchable(Watchable &&other):id(other.id),length(other.length),tags(other.tags) {
    other.length = 0;
    other.tags = {};
}
const long Watchable::getId() const
{
    return this->id;
}
int Watchable::getLength() const
{
    return this->length;
}
const vector<string>& Watchable::getTags() const
{
    return this->tags;
}





//*************************************Movie************************************

Movie::Movie(long id, const string &name, int length, const vector<string> &tags) :Watchable(id,length,tags), name(name)
{
}
Movie::~Movie(){}
Movie::Movie(const Movie &movie) :Watchable(movie), name(movie.name)
{
}
Movie* Movie::clone() const
{
    return new Movie(*this);
}
string Movie::getName()
{
    return this->name;
}
std::string Movie::toString() const
{
    return this->name;
}
Watchable* Movie::getNextWatchable(Session& s) const
{
    return s.getActiveUser()->searchForNextWatchable(s);
}
void Movie::setNextEpisodeId(int id)
{
    return;
}
Movie::Movie(Movie &&other):Watchable(other),name(other.name) {
}


//***************************************Episode**********************************
Episode::Episode(long id, const std::string &seriesName, int length, int season, int episode, const std::vector<std::string> &tags)
                        : Watchable(id,length,tags), seriesName(seriesName) ,season(season) , episode(episode),nextEpisodeId(-1)
{
}
Episode::~Episode(){}
Episode::Episode(const Episode &episode)
        : Watchable(episode)
        , seriesName(episode.seriesName),season(episode.season),episode(episode.episode),nextEpisodeId(episode.nextEpisodeId){
}
Episode* Episode::clone() const
{
    return new Episode(*this);
}
std::string Episode::getSeriesName() const
{
    return this->seriesName;
}
int Episode::getSeason() const
{
    return this->season;
}
int Episode::getEpisode() const
{
    return this->episode;
}
std::string Episode::toString() const
{
    return this->seriesName+" S"+to_string(getSeason())+"E"+to_string(getEpisode())+' ';
}
long Episode::getNextEpisodeId() const
{
    return this->nextEpisodeId;
}
Watchable* Episode::getNextWatchable(Session& s) const
{
    return s.getActiveUser()->getNextRecommendationForEpisode(this->getNextEpisodeId(),s);
}
void Episode::setNextEpisodeId(int id)
{
    this->nextEpisodeId=id;
}
Episode::Episode(Episode &&other):Watchable(other),seriesName(other.seriesName),season(other.season),episode(other.episode),nextEpisodeId(other.nextEpisodeId)
{
}

