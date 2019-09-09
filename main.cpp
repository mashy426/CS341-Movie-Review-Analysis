/*main.cpp*/

//
// Netflix movie review analysis.
//
// Shyam Patel (NetID: spate54)
// U. of Illinois, Chicago
// CS 341: Fall 2018
// Project 02
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <chrono>

using namespace std;


// class for Movie object
class Movie {
  private:
    // private data members
    int    movieID;
    string movieName;
    int    pubishYear;
    int    numReviews;
    double sumRatings;
    int    star[5];

  public:
    // constructor (member initialization list)
    Movie(int ID, string name, int year)
      : movieID(ID), movieName(name), pubishYear(year) {
      numReviews = 0;
      sumRatings = 0.0;
      for (int i = 0; i < 5; i++)
        star[i]  = 0;
    }

    // move constructor
    Movie(Movie&& other)
      : movieID   (move(other.movieID   )),
        movieName (move(other.movieName )),
        pubishYear(move(other.pubishYear)) {
      numReviews = 0;
      sumRatings = 0.0;
      for (int i = 0; i < 5; i++)
        star[i]  = 0;
    }

    // getters
    int    getMovieID()    const { return movieID;     }
    string getMovieName()  const { return movieName;   }
    int    getYear()       const { return pubishYear;  }
    int    getNumReviews() const { return numReviews;  }
    int    getStar(int n)  const { return star[n - 1]; }

    double getAvgRating()  const {
      if (numReviews == 0)
        return 0.0;
      else
        return sumRatings / numReviews;
    }//end getAvgRating()

    void incrementNumReviews() {
      numReviews++;
    }//end incrementNumReviews()

    void incrementStar(int rating) {
      star[rating - 1]++;
    }//end incrementStar()

    void addSumRatings(int rating) {
      sumRatings += rating;
    }//end addSumRatings()
};//end Movie class


// class for Review object
class Review {
  private:
    // private data members
    int    reviewID;
    int    movieID;
    int    userID;
    int    reviewRating;
    string reviewDate;

  public:
    // constructor (member initialization list)
    Review(int review, int movie, int user, int rating, string date)
      : reviewID(review), movieID(movie), userID(user),
        reviewRating(rating), reviewDate(date) { }

    // move constructor
    Review(Review&& other)
      : reviewID    (move(other.reviewID    )),
        movieID     (move(other.movieID     )),
        userID      (move(other.userID      )),
        reviewRating(move(other.reviewRating)),
        reviewDate  (move(other.reviewDate  )) { }

    // getters
    int    getReviewID() const { return reviewID;     }
    int    getMovieID()  const { return movieID;      }
    int    getUserID()   const { return userID;       }
    int    getRating()   const { return reviewRating; }
    string getDate()     const { return reviewDate;   }
};//end Review class


// check(): ensure filename exists and file is accessible
bool check(string filename) {
  ifstream file(filename); // input file stream

  if (!file.good()) {      // state of stream :
    cout << "Error reading file: " << filename << endl;
    return false;          //   fail
  }

  return true;             //   success
}//end check()


// readMovies(): parse thru lines in movies file and
//               store movies into movies map and movies vector
void readMovies(string moviesFN, unordered_map<int, Movie>& moviesMap,
                vector<int>& moviesVector) {
  string line, ID, name, year;              // string declarations
  int  count = 0;                           // count
  auto begin = chrono::steady_clock::now(); // begin time
  ifstream moviesFile(moviesFN);            // input file stream
  getline(moviesFile, line);                // discard header

  while (getline(moviesFile, line)) {       // parse thru lines :
    count++;                                //   increment count
    stringstream ss(line);                  //   string buffer
    getline(ss, ID,   ',');                 //   get movie ID
    getline(ss, name, ',');                 //   get movie name
    getline(ss, year     );                 //   get movie year

    // insert movie ID and new Movie object into movies map
    moviesMap.emplace(stoi(ID), Movie(stoi(ID), name, stoi(year)));
    moviesVector.push_back(stoi(ID));       // append movie ID to movies vector
  }//end loop

  moviesFile.close();                       // close stream
  auto end  = chrono::steady_clock::now();  // end time
  auto diff = end - begin;                  // calculate difference
  cout << ">> Reading movies... " << count
       << " [ time: " << chrono::duration<double, milli>(diff).count()
       << " ms ]" << endl;
}//end readMovies()


// readReviews(): parse thru lines in reviews file and
//                store reviews into reviews map
void readReviews(string reviewsFN, unordered_map<int, Review>& reviewsMap) {
  string line, rID, mID, uID, rating, date; // string declarations
  int  count = 0;                           // count
  auto begin = chrono::steady_clock::now(); // begin time
  ifstream reviewsFile(reviewsFN);          // input file stream
  getline(reviewsFile, line);               // discard header

  while (getline(reviewsFile, line)) {      // parse thru lines :
    count++;                                //   increment count
    stringstream ss(line);                  //   string buffer
    getline(ss, rID,    ',');               //   get review ID
    getline(ss, mID,    ',');               //   get movie ID
    getline(ss, uID,    ',');               //   get user ID
    getline(ss, rating, ',');               //   get review rating
    getline(ss, date       );               //   get review date

    // insert review ID and new Review object into reviews map
    reviewsMap.emplace(stoi(rID), Review(stoi(rID), stoi(mID), stoi(uID),
                                         stoi(rating), date));
  }//end loop

  reviewsFile.close();                      // close stream
  auto end  = chrono::steady_clock::now();  // end time
  auto diff = end - begin;                  // calculate difference
  cout << ">> Reading reviews... " << count
       << " [ time: " << chrono::duration<double, milli>(diff).count()
       << " ms ]" << endl;
}//end readReviews()


// processReviews(): update movie data to reflect total number
//                   of reviews and sum of ratings
void processReviews(const unordered_map<int, Review>& reviewsMap,
                    unordered_map<int, Movie>& moviesMap) {
  // iterate thru reviews map
  for (const auto& pair : reviewsMap) {
    int ID     = pair.second.getMovieID(); // movie ID
    int rating = pair.second.getRating();  // review rating
    Movie *m   = &moviesMap.at(ID);        // pointer to movie

    m->incrementNumReviews();              // increment # of reviews
    m->incrementStar(rating);              // increment # of star rating
    m->addSumRatings(rating);              // add rating to sum
  }//end loop
}//end processReviews()


// sortMovies(): sort movies vector primarily according to average rating
//               and, if equal, secondarily according to movie name
void sortMovies(const unordered_map<int, Movie>& moviesMap,
                vector<int>& moviesVector) {
  sort(moviesVector.begin(), moviesVector.end(),
       [&](const int& ID1, const int& ID2) {
         const Movie *m1 = &moviesMap.at(ID1); // pointer to movie 1
         const Movie *m2 = &moviesMap.at(ID2); // pointer to movie 2

         // primary sort by average rating
         if      (m1->getAvgRating() > m2->getAvgRating())
           return true;
         else if (m1->getAvgRating() < m2->getAvgRating())
           return false;
         else {
           // secondary sort by movie name
           if    (m1->getMovieName() < m2->getMovieName())
             return true;
           else
             return false;
         }
       });
}//end sortMovies()


// displayTopTenMovies(): print top ten movies by average rating
void displayTopTenMovies(unordered_map<int, Movie>& moviesMap,
                         const unordered_map<int, Review>& reviewsMap,
                         vector<int>& moviesVector) {
  auto begin = chrono::steady_clock::now(); // begin time
  processReviews(reviewsMap, moviesMap);    // process reviews
  sortMovies(moviesMap, moviesVector);      // sort movies

  cout << endl << ">> Top-10 Movies <<"  << endl << endl;
  cout << "Rank\tID\tReviews\tAvg\tName" << endl;

  for (int i = 0; i < 10; i++) {
    int ID   = moviesVector[i];             // movie ID
    Movie *m = &moviesMap.at(ID);           // pointer to movie

    cout << setprecision(6)    << i+1 << ".\t"
         << m->getMovieID()    << "\t"
         << m->getNumReviews() << "\t"
         << m->getAvgRating()  << "\t\'"
         << m->getMovieName()  << "\'" << endl;
  }//end loop

  auto end  = chrono::steady_clock::now();  // end time
  auto diff = end - begin;                  // calculate difference
  cout << "[ time: " << chrono::duration<double, milli>(diff).count()
       << " ms ]" << endl;
}//end displayTopTenMovies()


// searchMovies(): search for movie ID and, if found, print its information
bool searchMovies(const unordered_map<int, Movie>& moviesMap, int ID) {
  if (moviesMap.find(ID) == moviesMap.end()) { // movie not found
    cout << endl << "movie not found..." << endl;
    return false;
  }                                            // movie found :
  const Movie *m = &moviesMap.at(ID);          //   pointer to movie

  cout << setprecision(6)  << endl
       << "Movie:\t\t\'"   << m->getMovieName()  << "\'" << endl
       << "Year:\t\t"      << m->getYear()       << endl
       << "Avg rating:\t"  << m->getAvgRating()  << endl
       << "Num reviews:\t" << m->getNumReviews() << endl
       << " 1 star:\t"     << m->getStar(1)      << endl
       << " 2 stars:\t"    << m->getStar(2)      << endl
       << " 3 stars:\t"    << m->getStar(3)      << endl
       << " 4 stars:\t"    << m->getStar(4)      << endl
       << " 5 stars:\t"    << m->getStar(5)      << endl;

  return true;
}//end searchMovies()


// searchReviews(): search for review ID and, if found, print its information
bool searchReviews(const unordered_map<int, Review>& reviewsMap,
                   const unordered_map<int, Movie>& moviesMap, int ID) {
  if (reviewsMap.find(ID) == reviewsMap.end()) { // review not found
    cout << endl << "review not found..." << endl;
    return false;
  }                                              // review found :
  const Review *r = &reviewsMap.at(ID);          //   pointer to review
  int movieID     = r->getMovieID();             //   movie ID
  const Movie *m  = &moviesMap.at(movieID);      //   pointer to movie

  cout << endl
       << "Movie: "     << r->getMovieID()
       << " ("          << m->getMovieName() << ")" << endl
       << "Num stars: " << r->getRating()    << endl
       << "User id:   " << r->getUserID()    << endl
       << "Date:      " << r->getDate()      << endl;

  return true;
}//end searchReviews()


// displayMovieReviewInfo(): allow user search by movie or review ID
//                           and display its information
void displayMovieReviewInfo(unordered_map<int, Movie>& moviesMap,
                            unordered_map<int, Review>& reviewsMap) {
  string searchID;                        // search string
  cout << endl << ">> Movie and Review Information <<" << endl;

  for (;;) {                              // infinite loop
    cout << endl << "Please enter a movie ID (< 100,000), "
                 << "a review ID (>= 100,000), 0 to stop> ";
    cin >> searchID;                      //   user input
    int ID = stoi(searchID);              //   int conversion

    if      (ID == 0)                     //   stop  : '0' entered
      break;
    else if (ID < 0)                      //   error : invalid ID
      cout << endl << "**invalid id..." << endl;
    else if (ID < 100000) {               //   search movies
      if (!searchMovies(moviesMap, ID))
        continue;
    }
    else {                                //   search reviews
      if (!searchReviews(reviewsMap, moviesMap, ID))
        continue;
    }
  }//end loop
}//end displayMovieReviewInfo()


int main() {
  cout << "** Netflix Movie Review Analysis **" << endl << endl;

  //
  // input the filenames to process:
  //
  string moviesFN, reviewsFN;

  cin >> moviesFN;
  cin >> reviewsFN;

  cout << moviesFN << endl;
  cout << reviewsFN << endl << endl;

  //
  // TODO:
  //                                             // declarations :
  unordered_map<int, Movie> moviesMap;           //   movies map
  vector<int> moviesVector;                      //   movies vector
  unordered_map<int, Review> reviewsMap;         //   reviews map

  if (!check(moviesFN) || !check(reviewsFN))     // verify files are accessible
    return -1;

  readMovies(moviesFN, moviesMap, moviesVector); // read movies
  readReviews(reviewsFN, reviewsMap);            // read reviews

  // part 1 : top ten movies
  displayTopTenMovies(moviesMap, reviewsMap, moviesVector);

  // part 2 : movie + review info
  displayMovieReviewInfo(moviesMap, reviewsMap);

  //
  // done:
  //
  cout << endl << "** DONE! **" << endl << endl;

  return 0;
}
