//
// main.cpp
//
// WordSearch
// Garrett Heisey
// 1337.010 - Prof. Doug DeGroot
//
// changelog:
//              13Feb23 - Created program to take notes during class
//              14Feb23 - Finished the file reading aspect of the program, the matrix is now assigned to a 2d
//                        vector to be searched through, as well as having a vector filled with the names of the movies
//                        to search for containing spaces, and wrote a few functions to create a vector filled with the
//                        movie names without spaces
//              15Feb23 - Started coding the searching code for looking through the matrix for each movie title, ran into a lot
//                        of bugs and the code is not working very well at all
//              17Feb23 - Debugged my searching algorithm and it now works and is able to find all movies except for Napoleon
//                        Dynamite and Raising Arizona, but looking at the word search myself I can't seem to find them either.
//              19Feb23 - Created new vectors to store the X&Y coordinates where a movie was found, separation of functions into
//                        more functions with their own assignments, added direction to the output of when a movie was found,
//                        added a vector of words that were not found and display the contents of the vector at the end of the program,
//                        and finished function prototyping to allow functions to be called from anywhere in the program,
//
//
//
// notes:
//      I found that the files read differently on mac and windows, I assume that this is because of the difference in
//      new line characters in text files on the two operating systems. I was receieving differing output on mac vs windows due to a
//      for loop checking for an extra new line character on mac, and I fixed this by error checking on a windows pc and changing my code.
//      
//
//
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// Function prototyping

bool OpenFile(string fileName, ifstream& inputFile);
void SkipCommentLines(ifstream& inputFile);
void ReadFile(ifstream& inputFile, vector<vector<char>>& matrix, vector<string>& movieNamesWithSpaces);
string removeSpacesFromMovieTitle(string& str);
void SearchThroughMatrix(vector<vector<char>> matrix, vector<string> movieNamesWithoutSpaces, vector<string>& foundMovies,
                         vector<string>& notFoundMovies, vector<string>& foundMovieDirection, vector<string> movieNamesWithSpaces,
                         vector<int>& foundMovieXCoordinates, vector<int>& foundMovieYCoordinates);
bool titleMatches(vector<vector<char>> matrix, string movie, int secondLetterX, int secondLetterY, string direction,
                  int firstLetterX, int firstLetterY);
void FindAndPrintNotFoundMovies(vector<string> movieNamesWithSpaces, vector<string> foundMovies, vector<string> notFoundMovies);
void PrintFoundMovies(vector<string> foundMovies, vector<int> foundMovieXCoordinates, vector<int> foundMovieYCoordinates,
                      vector<string> foundMovieDirection);
void PrintMatrix(vector<vector<char>> matrix);


bool OpenFile(string fileName, ifstream& inputFile)
{
    string quit = "quit";
    
    cout << "Enter the name of the file to be opened: " << endl;
    cin >> fileName;
    
    inputFile.open(fileName);
    while(!inputFile && fileName != quit)
    {
        cout << "The file '" << fileName << "' was unable to be opened. Type quit or enter the name of the file to be opened: " << endl;
        cin >> fileName;
        inputFile.open(fileName);
    }
    if(fileName == quit)
    {
        return false;
    }
    return true;
}

void SkipCommentLines(ifstream& inputFile)
{
    string textLine;
    char ch;
    ch = inputFile.peek();
    while(ch == '#' || ch == '\n')
    {
        getline(inputFile,textLine);
        ch = inputFile.peek();
        //cout << "line containing '" << textLine << "' was skipped " << endl;
    }
}

void ReadFile(ifstream& inputFile, vector<vector<char>>& matrix, vector<string>& movieNamesWithSpaces)
{
    int count = 0;
    int count2 = 0;
    char ch;
    int matrixWidth;
    int matrixHeight;
    vector<char> row;
    bool matrixFull = false;
    string textLine;
    
    while(inputFile.peek()!=EOF)
    {
        ch = inputFile.peek();
        
        if(ch == '#' || ch == '\n')
        {
            SkipCommentLines(inputFile);
        }
        else if(toupper(ch)>=48 && toupper(ch)<=57) // if character is a digit
        {
            inputFile >> matrixWidth >> matrixHeight;
        }
        else if(toupper(ch)>=65 && toupper(ch)<=90 && !matrixFull) // if character is a letter
        {
            inputFile >> ch;
            row.push_back(ch);
            count++;
            
            if(count==18)
            {
                matrix.push_back(row);
                row.clear();
                count2++;
                if(count == 18 && count2 == 18)
                {
                    matrixFull = true;
                }
                count = 0;
            }
        }
        else if(toupper(ch)>=65 && toupper(ch)<=90 && matrixFull) // if character is a letter
        {
            getline(inputFile, textLine);
            movieNamesWithSpaces.push_back(textLine);
        }
        else
        {
            inputFile >> ch;
        }
    }
}

string removeSpacesFromMovieTitle(string& str)
{
    string newString = "";
    for(int i=0; i<str.length(); i++)
    {
        if(str.at(i) != ' ')
        {
            newString += toupper(str.at(i));
        }
    }
    return newString;
}

void SearchThroughMatrix(vector<vector<char>> matrix, vector<string> movieNamesWithoutSpaces, vector<string>& foundMovies,
                         vector<string>& notFoundMovies, vector<string>& foundMovieDirection, vector<string> movieNamesWithSpaces,
                         vector<int>& foundMovieXCoordinates, vector<int>& foundMovieYCoordinates)
{
    string direction;
    
    // loop through each movie title in list
    for(int i=0; i<movieNamesWithoutSpaces.size(); i++)
    {
        // loop through matrix height
        for(int k=0; k<matrix.size(); k++)
        {
            // loop through matrix width
            for(int l=0; l<matrix[k].size(); l++)
            {
                // if first letter found at current location
                if(matrix[k][l]==movieNamesWithoutSpaces[i].at(0))
                {
                    // if going east doesn't put us out of bounds and second letter is found
                    if(l!=matrix[k].size()-1 && matrix[k][l+1]==movieNamesWithoutSpaces[i].at(1))
                    {
                        direction = "E";
                        // see if the rest of the string is found in the same direction
                        if(titleMatches(matrix, movieNamesWithoutSpaces[i], l+1, k, direction, l, k))
                        {
                            foundMovies.push_back(movieNamesWithSpaces[i]);
                            foundMovieDirection.push_back(direction);
                            foundMovieXCoordinates.push_back(l);
                            foundMovieYCoordinates.push_back(k);
                        }
                    }
                    // if going southeast doesn't put us out of bounds and second letter is found
                    if(l!=matrix[k].size()-1 && k!=matrix.size()-1 && matrix[k+1][l+1]==movieNamesWithoutSpaces[i].at(1))
                    {
                        direction = "SE";
                        // see if the rest of the string is found in the same direction
                        if(titleMatches(matrix, movieNamesWithoutSpaces[i], l+1, k+1, direction, l, k))
                        {
                            foundMovies.push_back(movieNamesWithSpaces[i]);
                            foundMovieDirection.push_back(direction);
                            foundMovieXCoordinates.push_back(l);
                            foundMovieYCoordinates.push_back(k);
                        }
                    }
                    // if going south doesn't put us out of bounds and second letter is found
                    if(k!=matrix.size()-1 && matrix[k+1][l]==movieNamesWithoutSpaces[i].at(1))
                    {
                        direction = "S";
                        // see if the rest of the string is found in the same direction
                        if(titleMatches(matrix, movieNamesWithoutSpaces[i], l, k+1, direction, l, k))
                        {
                            foundMovies.push_back(movieNamesWithSpaces[i]);
                            foundMovieDirection.push_back(direction);
                            foundMovieXCoordinates.push_back(l);
                            foundMovieYCoordinates.push_back(k);
                        }
                    }
                    // if going southwest doesn't put us out of bounds and second letter is found
                    if(l!=0 && k!=matrix.size()-1 && matrix[k+1][l-1]==movieNamesWithoutSpaces[i].at(1))
                    {
                        direction = "SW";
                        // see if the rest of the string is found in the same direction
                        if(titleMatches(matrix, movieNamesWithoutSpaces[i], l-1, k+1, direction, l, k))
                        {
                            foundMovies.push_back(movieNamesWithSpaces[i]);
                            foundMovieDirection.push_back(direction);
                            foundMovieXCoordinates.push_back(l);
                            foundMovieYCoordinates.push_back(k);
                        }
                    }
                    // if going west doesn't put us out of bounds and second letter is found
                    if(l!=0 && matrix[k][l-1]==movieNamesWithoutSpaces[i].at(1))
                    {
                        direction = "W";
                        // see if the rest of the string is found in the same direction
                        if(titleMatches(matrix, movieNamesWithoutSpaces[i], l-1, k, direction, l, k))
                        {
                            foundMovies.push_back(movieNamesWithSpaces[i]);
                            foundMovieDirection.push_back(direction);
                            foundMovieXCoordinates.push_back(l);
                            foundMovieYCoordinates.push_back(k);
                        }
                    }
                    // if going northwest doesn't put us out of bounds and second letter is found
                    if(l!=0 && k!=0 && matrix[k-1][l-1]==movieNamesWithoutSpaces[i].at(1))
                    {
                        direction = "NW";
                        // see if the rest of the string is found in the same direction
                        if(titleMatches(matrix, movieNamesWithoutSpaces[i], l-1, k-1, direction, l, k))
                        {
                            foundMovies.push_back(movieNamesWithSpaces[i]);
                            foundMovieDirection.push_back(direction);
                            foundMovieXCoordinates.push_back(l);
                            foundMovieYCoordinates.push_back(k);
                        }
                    }
                    // if going north doesn't put us out of bounds and second letter is found
                    if(k!=0 && matrix[k-1][l]==movieNamesWithoutSpaces[i].at(1))
                    {
                        direction = "N";
                        // see if the rest of the string is found in the same direction
                        if(titleMatches(matrix, movieNamesWithoutSpaces[i], l, k-1, direction, l, k))
                        {
                            foundMovies.push_back(movieNamesWithSpaces[i]);
                            foundMovieDirection.push_back(direction);
                            foundMovieXCoordinates.push_back(l);
                            foundMovieYCoordinates.push_back(k);
                        }
                    }
                    // if going northeast doesn't put us out of bounds and second letter is found
                    if(k!=0 && l!=matrix[k].size()-1 && matrix[k-1][l+1]==movieNamesWithoutSpaces[i].at(1))
                    {
                        direction = "NE";
                        // see if the rest of the string is found in the same direction
                        if(titleMatches(matrix, movieNamesWithoutSpaces[i], l+1, k-1, direction, l, k))
                        {
                            foundMovies.push_back(movieNamesWithSpaces[i]);
                            foundMovieDirection.push_back(direction);
                            foundMovieXCoordinates.push_back(l);
                            foundMovieYCoordinates.push_back(k);
                        }
                    }
                    // else second letter was not found
                    else
                    {
                        
                    }
                }
            }
        }
    }
}

bool titleMatches(vector<vector<char>> matrix, string movie, int secondLetterX, int secondLetterY, string direction,
                  int firstLetterX, int firstLetterY)
{
    bool charsMatch = true;
    
    //cout << "Checking for movie " << movie << " at [" << firstLetterX << "][" << firstLetterY << "] going " << direction << endl;
    
    // keep traveling in direction of offsets and see if word is found, while being sure to not go out of bounds
    for(int i=1; i<movie.length()-1; i++)
    {
        // if we don't go off the board
        if(direction=="E")
        {
            if(secondLetterX+i==matrix[secondLetterX].size())
            {
                //cout << "out of bounds" << endl;
                charsMatch = false;
                break;
            }
            else if(matrix[secondLetterY][secondLetterX+i]!=movie.at(i+1))
            {
                //cout << "chars at [" << secondLetterX+i << "][" << secondLetterY << "] dont match" << endl;
                charsMatch = false;
                break;
            }
            else
            {
                //cout << "continued" << endl;
                continue;
            }
        }
        else if(direction=="SE")
        {
            if(secondLetterX+i==matrix[secondLetterX].size() || secondLetterY+i==matrix.size())
            {
                //cout << "out of bounds" << endl;
                charsMatch = false;
                break;
            }
            else if(matrix[secondLetterY+i][secondLetterX+i]!=movie.at(i+1))
            {
                //cout << "chars dont match" << endl;
                charsMatch = false;
                break;
            }
            else
            {
                //cout << "continued" << endl;
                continue;
            }
        }
        else if(direction=="S")
        {
            if(secondLetterY+i==matrix.size())
            {
                //cout << "out of bounds" << endl;
                charsMatch = false;
                break;
            }
            else if(matrix[secondLetterY+i][secondLetterX]!=movie.at(i+1))
            {
                //cout << "chars dont match" << endl;
                charsMatch = false;
                break;
            }
            else
            {
                //cout << "continued" << endl;
                continue;
            }
        }
        else if(direction=="SW")
        {
            if(secondLetterX-i==-1 || secondLetterY+i==matrix.size())
            {
                //cout << "out of bounds" << endl;
                charsMatch = false;
                break;
            }
            else if(matrix[secondLetterY+i][secondLetterX-i]!=movie.at(i+1))
            {
                //cout << "chars dont match" << endl;
                charsMatch = false;
                break;
            }
            else
            {
                //cout << "continued" << endl;
                continue;
            }
        }
        else if(direction=="W")
        {
            if(secondLetterX-i==-1)
            {
                //cout << "out of bounds" << endl;
                charsMatch = false;
                break;
            }
            else if(matrix[secondLetterY][secondLetterX-i]!=movie.at(i+1))
            {
                //cout << "chars dont match" << endl;
                charsMatch = false;
                break;
            }
            else
            {
                //cout << "continued" << endl;
                continue;
            }
        }
        else if(direction=="NW")
        {
            if(secondLetterX-i==-1 || secondLetterY-i==-1)
            {
                //cout << "out of bounds" << endl;
                charsMatch = false;
                break;
            }
            else if(matrix[secondLetterY-i][secondLetterX-i]!=movie.at(i+1))
            {
                //cout << "chars dont match" << endl;
                charsMatch = false;
                break;
            }
            else
            {
                //cout << "continued" << endl;
                continue;
            }
        }
        else if(direction=="N")
        {
            if(secondLetterY-i==-1)
            {
                //cout << "out of bounds" << endl;
                charsMatch = false;
                break;
            }
            else if(matrix[secondLetterY-i][secondLetterX]!=movie.at(i+1))
            {
                //cout << "chars dont match" << endl;
                charsMatch = false;
                break;
            }
            else
            {
                //cout << "continued" << endl;
                continue;
            }
        }
        else // NE
        {
            if(secondLetterX+i==matrix[secondLetterX].size() || secondLetterY-i==-1)
            {
                //cout << "out of bounds" << endl;
                charsMatch = false;
                break;
            }
            else if(matrix[secondLetterY-i][secondLetterX+i]!=movie.at(i+1))
            {
                //cout << "chars dont match" << endl;
                charsMatch = false;
                break;
            }
            else
            {
                //cout << "continued" << endl;
                continue;
            }
        }
    }
    
    if(charsMatch)
    {
        //cout << "movie " << movie << " found at [" << firstLetterX << "][" << firstLetterY << "]*********" << endl;
        return true;
    }
    else
    {
        return false;
    }
}

void PrintMatrix(vector<vector<char>> matrix)
{
    for(int i=0;i<matrix.size();i++)
    {
        for(int j=0;j<matrix[i].size();j++)
        {
            cout << matrix[i][j];
        }
        cout << endl;
    }
}

void PrintFoundMovies(vector<string> foundMovies, vector<int> foundMovieXCoordinates, vector<int> foundMovieYCoordinates,
                      vector<string> foundMovieDirection)
{
    for(int i=0;i<foundMovies.size();i++)
    {
        cout << foundMovies[i] << " found at " << foundMovieYCoordinates[i]+1<< ", " << foundMovieXCoordinates[i]+1 <<
        ": (direction = " << foundMovieDirection[i]  << ")" << endl;
    }
}

void FindAndPrintNotFoundMovies(vector<string> movieNamesWithSpaces, vector<string> foundMovies, vector<string> notFoundMovies)
{
    for(int i=0;i<movieNamesWithSpaces.size();i++)
    {
        bool flag = false;
        for(int j=0;j<foundMovies.size();j++)
        {
            if(movieNamesWithSpaces[i]==foundMovies[j])
            {
                flag = true;
            }
            
        }
        if(flag == false)
        {
            notFoundMovies.push_back(movieNamesWithSpaces[i]);
        }
    }
    
    cout << "Couldn't find these movies:" << endl;
    
    for(int i=0;i<notFoundMovies.size();i++)
    {
        cout << notFoundMovies[i] << endl;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------


int main()
{
    string fileName;
    ifstream inputFile;
    vector<vector<char>> matrix;
    vector<string> movieNamesWithSpaces;
    vector<string> movieNamesWithoutSpaces;
    vector<string> foundMovies;
    vector<string> notFoundMovies;
    vector<string> foundMovieDirection;
    vector<int> foundMovieXCoordinates;
    vector<int> foundMovieYCoordinates;
    
    if(OpenFile(fileName, inputFile))
    {
        ReadFile(inputFile, matrix, movieNamesWithSpaces);
        
        for(int i=0;i<movieNamesWithSpaces.size(); i++)
        {
            string currentMovie = movieNamesWithSpaces[i];
            currentMovie = removeSpacesFromMovieTitle(currentMovie);
            movieNamesWithoutSpaces.push_back(currentMovie);
        }
        
        SearchThroughMatrix(matrix, movieNamesWithoutSpaces, foundMovies, notFoundMovies, foundMovieDirection, movieNamesWithSpaces,
                            foundMovieXCoordinates, foundMovieYCoordinates);
    }
    
    PrintMatrix(matrix);
    cout << endl;
    
    PrintFoundMovies(foundMovies, foundMovieXCoordinates, foundMovieYCoordinates, foundMovieDirection);
    cout << endl;
    
    FindAndPrintNotFoundMovies(movieNamesWithSpaces, foundMovies, notFoundMovies);
    
    return 0;
}
