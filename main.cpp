#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "helper.h"
#include "programs.h"
#include "dropdown.h"
#include <cstdlib>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
using namespace std;

// loads all the programs from the dataset, all 8000 something of them
vector<studyAbroadProgram> loadPrograms(string filename){
    vector<studyAbroadProgram> programs;
    ifstream file(filename);

    // check to make sure the file is open
    if(!file.is_open()){
        cerr << "Error: file not open" << filename << endl;
        return programs;
    }

    string line;
    getline(file, line);

    while(getline(file, line)){
        stringstream ss(line);
        string name, city, country, region, advisor, about, terms, college, sapa, gpaStr, costStr, credit;

        // get all the information
        getline(ss, name, ',');
        getline(ss, city, ',');
        getline(ss, country, ',');
        getline(ss, region, ',');
        getline(ss, terms, ',');
        getline(ss, gpaStr, ',');
        getline(ss, advisor, ',');
        getline(ss, college, ',');
        getline(ss, credit, ',');
        getline(ss, costStr, ',');
        getline(ss, sapa, ',');
        getline(ss, about);

        // create a program
        studyAbroadProgram program(name, city, country, region, terms, gpaStr, advisor, college, credit, costStr, sapa, about);

        // add it to the vector
        programs.push_back(program);
    }

    // return the vector of all the programs
    return programs;
}

// there are many costs in our dataset that are
// "N/A" because there was no cost listed on the website
// This function determines if there is an actual price
bool isValidCost(string str){
    if(str == "N/A"){
        return false;
    }
    for(char c : str){
        if(!isdigit(c)){
            return false;
        }
    }
    return true;
}

// for the sake of this project, we removed the programs without a cost
// because we are prioritizing cost
vector<studyAbroadProgram> removeInvalidProgram(vector<studyAbroadProgram>& programs){
    vector<studyAbroadProgram> validPrograms;
    for(auto program : programs){
        if(isValidCost(program.cost)){
            validPrograms.push_back(program);
        }
    }

    // return only programs with a cost
    return validPrograms;
}

// helper for the quick sort
int partitionCost(vector<studyAbroadProgram>& programs, int low, int high){
    int pivot = stoi(programs[low].cost);
    int up = low;
    int down = high;
    while(up < down){
        while(up <= high && stoi(programs[up].cost) <= pivot){
            up++;
        }
        while(down >= low && stoi(programs[down].cost) > pivot){
            down--;
        }
        if(up < down){
            swap(programs[up], programs[down]);
        }
    }
    swap(programs[low], programs[down]);
    return down;
}

// quick sort algorithm
void quickSortCost(vector<studyAbroadProgram>& programs, int low, int high){
    if(low < high){
        int pivotIndex = partitionCost(programs, low, high);
        quickSortCost(programs, low, pivotIndex-1);
        quickSortCost(programs, pivotIndex + 1, high);
    }
}

// heap sorting
//get vector of cost through using vector<studyAbroadProgram>
void heapify(vector<studyAbroadProgram>& heap, int heapSize, int root) {
    int minIndex = root;
    int leftIndex = 2 * root + 1;
    int rightIndex = 2 * root + 2;

    if (leftIndex < heapSize && stoi(heap[leftIndex].cost) < stoi(heap[minIndex].cost))
        minIndex = leftIndex;

    if (rightIndex < heapSize && stoi(heap[rightIndex].cost) < stoi(heap[minIndex].cost))
        minIndex = rightIndex;

    if (minIndex != root) {
        swap(heap[root], heap[minIndex]);
        heapify(heap, heapSize, minIndex);
    }
}

// Builds the min-heap from the array and sorts
vector<studyAbroadProgram> heapSort(vector<studyAbroadProgram> programs, int costRestraint) {
    programs = removeInvalidProgram(programs);
    int size = programs.size();
    vector<studyAbroadProgram> sorted;

    // Build min heap
    for (int i = size/2 - 1; i >= 0; i--) {
        heapify(programs, size, i);
    }

    // Extract elements one by one
    for (int i = size-1; i >= 0; i--) {
        if (stoi(programs[0].cost) <= costRestraint) {
            sorted.push_back(programs[0]);
        }
        swap(programs[0], programs[i]);
        heapify(programs, i, 0);
    }

    return sorted;
}

// programs are sorted by cost using quicksort, then we only want the programs
// with a cost below the max cost specified by the user
vector<studyAbroadProgram> filteredProgramsByCost(vector<studyAbroadProgram> programs, int costRestraint){
    programs = removeInvalidProgram(programs);
    quickSortCost(programs, 0, programs.size() - 1);
    vector<studyAbroadProgram> filtered;
    for(auto& p : programs){
        if(stoi(p.cost) <= costRestraint){
            filtered.push_back(p);
        }
    }

    // return only programs with costs below the max cost specified by user
    return filtered;
}

// already filtered by cost, find programs that match the country specified by user
vector<studyAbroadProgram> programsByCountry(vector<studyAbroadProgram> programs, string targetCountry){
    vector<studyAbroadProgram> validPrograms;
    for(auto program : programs){
        if(program.country == targetCountry){
            validPrograms.push_back(program);
        }
    }
    return validPrograms;
}

// already filtered by cost, find programs that match the gpa specified by user
vector<studyAbroadProgram> programsByGPA(vector<studyAbroadProgram> programs, string gpa){
    vector<studyAbroadProgram> validPrograms;
    for(auto program : programs){
        if(stof(gpa) >= stof(program.gpa)){
            validPrograms.push_back(program);
        }
    }
    return validPrograms;
}

int main() {
    // load of the programs from the dataset
    vector<studyAbroadProgram> programs = loadPrograms("Project3DSA.csv");

    // render window with width and height
    int width = 1200;
    int height = 800;
    sf::RenderWindow welcomeWindow(sf::VideoMode(width, height), "Study Abroad Program Finder");

    // load times font
    sf::Font font;
    if (!font.loadFromFile("times.ttf")) {  // Ensure this path is correct
        cerr << "Failed to load font." << endl;
        return -1;
    }

    //create welcome text
    sf::Text welcomeText;
    welcomeText.setFont(font);
    welcomeText.setString("Welcome to the UF Study Abroad Program Search!");
    welcomeText.setCharacterSize(50); // font size
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setStyle(sf::Text::Bold);

    // Center the text on the screen
    sf::FloatRect welcomeTextRect = welcomeText.getLocalBounds();
    welcomeText.setOrigin(welcomeTextRect.left + welcomeTextRect.width / 2.0f,
                          (welcomeTextRect.top + welcomeTextRect.height / 2.0f)+250);
    welcomeText.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f));

    // load logo png
    sf::Texture logoTexture;
    if (!logoTexture.loadFromFile("sas.png")){
        cerr <<"Failed to load image." << endl;
    }

    // create logo sprite
    sf::Sprite logoSprite;
    logoSprite.setTexture(logoTexture);
    logoSprite.setScale(0.7f, 0.7f);
    sf::FloatRect logoRect = logoSprite.getLocalBounds();
    logoSprite.setOrigin(logoRect.width / 2.0f, logoRect.height / 2.0f);
    logoSprite.setPosition(sf::Vector2f(width / 2.0f, (height / 2.150f)));

    // create question text
    sf::Text questionText;
    questionText.setFont(font);
    questionText.setString("Do you want to study abroad?");
    questionText.setCharacterSize(48);
    questionText.setFillColor(sf::Color::White);
    questionText.setStyle(sf::Text::Bold);
    sf::FloatRect questionRect = questionText.getLocalBounds();
    questionText.setOrigin(questionRect.left + questionRect.width / 2.0f,
                           questionRect.top + questionRect.height / 2.0f);
    questionText.setPosition(width / 2.0f, height * 0.75f);

    // create rectangle for yes button
    sf::RectangleShape yesButton(sf::Vector2f(300, 80));
    yesButton.setFillColor(sf::Color(249, 115, 22));
    yesButton.setOrigin(150, 40);
    yesButton.setPosition(width / 2.0f - 200, height * 0.87f);

    // create yes text
    sf::Text yesText;
    yesText.setFont(font);
    yesText.setString("Yes!");
    yesText.setCharacterSize(32);
    yesText.setFillColor(sf::Color::White);
    sf::FloatRect yesTextRect = yesText.getLocalBounds();
    yesText.setOrigin(yesTextRect.left + yesTextRect.width / 2.0f, yesTextRect.top + yesTextRect.height / 2.0f);
    yesText.setPosition(yesButton.getPosition());

    // create rectangle for no button
    sf::RectangleShape noButton(sf::Vector2f(300, 80));
    noButton.setFillColor(sf::Color(249, 115, 22));
    noButton.setOrigin(150, 40);
    noButton.setPosition(width / 2.0f + 200, height * 0.87f);

    // create no text
    sf::Text noText;
    noText.setFont(font);
    noText.setString("No :(");
    noText.setCharacterSize(32);
    noText.setFillColor(sf::Color::White);
    sf::FloatRect noTextRect = noText.getLocalBounds();
    noText.setOrigin(noTextRect.left + noTextRect.width / 2.0f, noTextRect.top + noTextRect.height / 2.0f);
    noText.setPosition(noButton.getPosition());

    // vector of all the occupied areas
    std::vector<sf::FloatRect> occupiedAreas = {
            yesButton.getGlobalBounds(),
            questionText.getGlobalBounds(),
            logoSprite.getGlobalBounds(),
            welcomeText.getGlobalBounds()
    };

    // welcome-window loop
    while (welcomeWindow.isOpen()) {
        // create event
        sf::Event event;

        while (welcomeWindow.pollEvent(event)) {
            // if you hit the x, close the window
            if (event.type == sf::Event::Closed) {
                welcomeWindow.close();
            }

            // if mouse button is pressed
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                    // if yes button clicked
                    if (yesButton.getGlobalBounds().contains(mousePos)) {
                        welcomeWindow.close();
                    }

                    // if no button clicked, but should not be possible
                    if (noButton.getGlobalBounds().contains(mousePos)) {
                        cout << "No button clicked!" << endl;
                    }
                }
            }

            // if mouse is moved
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));

                // if mouse moved over no button
                if (noButton.getGlobalBounds().contains(mousePos)) {
                    // find a random unoccupied position
                    sf::Vector2f newPos = getRandomUnoccupiedPosition(width, height, noButton.getSize(), occupiedAreas);

                    // move the no button to new position
                    noButton.setPosition(newPos);
                    noText.setPosition(newPos);
                }
            }
        }

        // draw everything
        welcomeWindow.clear(sf::Color(30, 58, 138));
        welcomeWindow.draw(logoSprite);
        welcomeWindow.draw(welcomeText);
        welcomeWindow.draw(questionText);
        welcomeWindow.draw(yesButton);
        welcomeWindow.draw(noButton);
        welcomeWindow.draw(yesText);
        welcomeWindow.draw(noText);
        welcomeWindow.display();
    }

    // render information window
    sf::RenderWindow informationWindow(sf::VideoMode(width, height), "Information");

    // set up information text
    sf::Text informationText;
    informationText.setFont(font);
    informationText.setString("Please fill in the information below.");
    informationText.setCharacterSize(50); // font size
    informationText.setFillColor(sf::Color::White);
    informationText.setStyle(sf::Text::Bold);

    // set up college text
    sf::Text college;
    college.setFont(font);
    college.setString("College");
    college.setCharacterSize(30); // font size
    college.setFillColor(sf::Color::White);
    college.setStyle(sf::Text::Bold);
    college.setPosition(sf::Vector2f(50, 150));

    // set up region text
    sf::Text region;
    region.setFont(font);
    region.setString("Desired Region");
    region.setCharacterSize(30); // font size
    region.setFillColor(sf::Color::White);
    region.setStyle(sf::Text::Bold);
    region.setPosition(sf::Vector2f(350, 150));

    // set up term text
    sf::Text term;
    term.setFont(font);
    term.setString("Desired Term");
    term.setCharacterSize(30); // font size
    term.setFillColor(sf::Color::White);
    term.setStyle(sf::Text::Bold);
    term.setPosition(sf::Vector2f(650, 150));

    // set up gpa text
    sf::Text gpa;
    gpa.setFont(font);
    gpa.setString("GPA");
    gpa.setCharacterSize(30); // font size
    gpa.setFillColor(sf::Color::White);
    gpa.setStyle(sf::Text::Bold);
    gpa.setPosition(sf::Vector2f(950, 150));

    // set up country text
    sf::Text country;
    country.setFont(font);
    country.setString("Country");
    country.setCharacterSize(30); // font size
    country.setFillColor(sf::Color::White);
    country.setStyle(sf::Text::Bold);
    country.setPosition(sf::Vector2f(50, 450));

    // set up major text
    sf::Text major;
    major.setFont(font);
    major.setString("Major");
    major.setCharacterSize(30); // font size
    major.setFillColor(sf::Color::White);
    major.setStyle(sf::Text::Bold);
    major.setPosition(sf::Vector2f(350, 450));

    // set up cost text
    sf::Text cost;
    cost.setFont(font);
    cost.setString("Maximum Cost");
    cost.setCharacterSize(30); // font size
    cost.setFillColor(sf::Color::White);
    cost.setStyle(sf::Text::Bold);
    cost.setPosition(sf::Vector2f(650, 450));

    // set up credits text
    sf::Text credit;
    credit.setFont(font);
    credit.setString("Desired Credits");
    credit.setCharacterSize(30); // font size
    credit.setFillColor(sf::Color::White);
    credit.setStyle(sf::Text::Bold);
    credit.setPosition(sf::Vector2f(950, 450));

    // center the information text on the screen
    sf::FloatRect informationTextRect = informationText.getLocalBounds();
    informationText.setOrigin(informationTextRect.left + informationTextRect.width / 2.0f,
                          (informationTextRect.top + informationTextRect.height / 2.0f)+300);
    informationText.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f));

    // create dropdown for college
    vector<string> options = {"CALS", "Arts", "Business", "Dentistry", "Design", "Education", "Engineering", "Health", "Journalism", "Law", "CLAS", "Medicine", "Nursing", "Public Health", "Veternary"};
    dropdown dropdown(50, 200, 200, 30, options);

    // create dropdown for region
    vector<string> options2 = {"Asia", "Europe", "Africa", "North America", "South America", "Australia/Pacific Islands"};
    class dropdown dropdown2(350, 200, 200, 30, options2);

    // create dropdown for term
    vector<string> options3 = {"Fall", "Spring", "Summer A", "Summer B", "Summer C"};
    class dropdown dropdown3(650, 200, 200, 30, options3);

    // create dropdown for gpa
    vector<string> options4 = {"2.5", "2.6", "2.7", "2.8", "2.9", "3.0", "3.1", "3.2", "3.3", "3.4", "3.5", "3.6", "3.7", "3.8", "3.9", "4.0"};
    class dropdown dropdown4(950, 200, 200, 30, options4);

    // create dropdown for country
    vector<string> options5 = {
            "Afghanistan", "Albania", "Algeria", "Andorra", "Angola",
            "Antigua and Barbuda", "Argentina", "Armenia", "Australia", "Austria",
            "Azerbaijan", "Bahamas", "Bahrain", "Bangladesh", "Barbados",
            "Belarus", "Belgium", "Belize", "Benin", "Bolivia",
            "Bosnia and Herzegovina", "Botswana", "Brazil", "Brunei", "Bulgaria",
            "Burkina Faso", "Burma", "Burundi", "Cabo Verde", "Cambodia",
            "Cameroon", "Canada", "Cayman Islands", "Central African Republic", "Chad",
            "Chile", "China", "Colombia", "Comoros", "Cook Islands",
            "Costa Rica", "Côte d’Ivoire", "Croatia", "Cuba", "Cyprus",
            "Czechia", "Democratic Republic of the Congo", "Denmark", "Djibouti", "Dominica",
            "Dominican Republic", "Ecuador", "Egypt", "El Salvador", "Equatorial Guinea",
            "Eritrea", "Estonia", "Eswatini", "Ethiopia", "Fiji",
            "Finland", "France", "Gabon", "Gambia", "Georgia",
            "Germany", "Ghana", "Greece", "Grenada", "Guatemala",
            "Guinea", "Guinea-Bissau", "Guyana", "Haiti", "Honduras",
            "Hungary", "Iceland", "India", "Indonesia", "Iran",
            "Iraq", "Ireland", "Israel", "Italy", "Jamaica",
            "Japan", "Jordan", "Kazakhstan", "Kenya", "Korea",
            "Kosovo", "Kuwait", "Kyrgyzstan", "Laos", "Latvia",
            "Lebanon", "Lesotho", "Liberia", "Libya", "Liechtenstein",
            "Lithuania", "Luxembourg", "Madagascar", "Malawi", "Malaysia",
            "Maldives", "Mali", "Malta", "Marshall Islands", "Mauritania",
            "Mauritius", "Mexico", "Micronesia", "Moldova", "Monaco",
            "Mongolia", "Montenegro", "Morocco", "Mozambique", "Namibia",
            "Nauru", "Nepal", "Netherlands", "New Zealand", "Nicaragua",
            "Niger", "Nigeria", "Niue", "North Macedonia", "Norway",
            "Oman", "Pakistan", "Palau", "Panama", "Papua New Guinea",
            "Paraguay", "Peru", "Philippines", "Poland", "Portugal",
            "Qatar", "Republic of Korea", "Republic of the Congo", "Romania", "Russia",
            "Rwanda", "Saint Kitts and Nevis", "Saint Lucia", "Saint Vincent and the Grenadines", "Samoa",
            "San Marino", "São Tomé and Príncipe", "Saudi Arabia", "Senegal", "Serbia",
            "Seychelles", "Sierra Leone", "Singapore", "Slovakia", "Slovenia",
            "Solomon Islands", "Somalia", "South Africa", "South Sudan", "Spain",
            "Sri Lanka", "Sudan", "Suriname", "Sweden", "Switzerland",
            "Syria", "Tajikistan", "Tanzania", "Thailand", "Timor-Leste",
            "Togo", "Tonga", "Trinidad and Tobago", "Tunisia", "Turkey",
            "Turkmenistan", "Tuvalu", "Uganda", "Ukraine", "United Arab Emirates",
            "United Kingdom", "Uruguay", "Uzbekistan", "Vanuatu", "Venezuela",
            "Vietnam", "Yemen", "Zambia", "Zimbabwe"
    };
    class dropdown dropdown5(50, 500, 200, 30, options5);

    // create dropdown for credits
    vector<string> options6 = {"1", "2", "3", "4", "5",  "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
    class dropdown dropdown6(950, 500, 200, 30, options6);

    // create dropdown for cost
    vector<string> options7 = {"5000",  "6000", "7000", "8000", "9000", "10000", "11000", "12000", "13000", "14000", "15000"};
    class dropdown dropdown7(650, 500, 200, 30, options7);

    // create dropdown for major
    vector<string> options8 = {
            "Acting",
            "Advertising",
            "African American Studies",
            "African Languages",
            "Agricultural and Biological Engineering",
            "Agricultural Education and Communication",
            "Agricultural Operations Management",
            "Animal Science",
            "Anthropology",
            "Applied Pysiology and Kinesiology",
            "Architecture",
            "Arabic",
            "Art Education",
            "Art History",
            "Arts in Medicine Online",
            "Arts in Public Health Online",
            "Astronomy and Astrophisics",
            "BFA Theatre Production",
            "Biological Engineering",
            "Biology",
            "Biomedical Engineering",
            "Botany",
            "Brass",
            "Business Administration",
            "Chemical Engineering",
            "Chemistry",
            "Chinese",
            "Classical Studies",
            "Coastal and Oceanographic Engineering",
            "Communication Sciences and Disorders",
            "Combination",
            "Composition, Theory and Technology",
            "Computer Engineering",
            "Computer Science",
            "Conducting",
            "Construction",
            "Criminology",
            "Dance",
            "Dance in Medicine",
            "Data Science",
            "Digital Arts and Sciences",
            "Dietetics",
            "Dual Languages",
            "Early Childhood Education",
            "Economics",
            "Education Sciences",
            "Eletrical and Computer Engineering",
            "Elementary Education",
            "Endodontics",
            "Engineering Education",
            "Entomology and Nemotology",
            "Environmental Engineering Sciences",
            "Environmental Management in Agriculture and Natural Resources",
            "Environmental Science",
            "Exhibition Venues",
            "Family, Youth and Community Sciences",
            "Finance",
            "Food and Resource Economics",
            "Food Science",
            "Foreign Languages and Literatures",
            "Forest Resources and Conservation",
            "French and Francophone Studies",
            "Geography",
            "Geology",
            "German",
            "Graphic Design/Design & Visual Communications",
            "Health Education",
            "Health Science",
            "Hebrew",
            "History",
            "Hispanic and Latin American Languages, Literatures and Linguistics",
            "Human-Centered Computing",
            "Information Systems",
            "Interior Design",
            "Interdisciplinary Studies",
            "International Studies",
            "Italian",
            "Jazz Studies",
            "Japanese",
            "Jewish Studies",
            "Journalism",
            "Juris Doctor",
            "Keyboard",
            "Landscape Architecture",
            "LLM in International Taxation",
            "LLM in Taxation",
            "LLM in US Law",
            "Linguistics",
            "Management",
            "Marine Sciences",
            "Marketing",
            "Materials Science and Engineering",
            "Mathematics",
            "Media Production, Management, and Technology",
            "Medicine",
            "Meteorology",
            "Microbiology and Cell Science",
            "Musical Theatre",
            "Museum Studies",
            "Music and MSM Business Management",
            "Music Business and Entrepreneurship",
            "Music Education",
            "Music for Pre-Health Professional",
            "Music in Combination with Outside Field",
            "Music in Medicine",
            "Musicology and Ethnomusicology",
            "Natural Resource Conservation",
            "Nuclear Engineering",
            "Nursing",
            "Nutritional Sciences",
            "Operative & Esthetic Dentistry",
            "Orthodontics",
            "Periodontics",
            "Percussion",
            "Philosophy",
            "Physics",
            "Plant Science",
            "Political Science",
            "Portuguese",
            "Pre-Professional Majors",
            "Professional Development",
            "Prosthodontics",
            "Psychology",
            "Public Health",
            "Public Relations",
            "Religion",
            "Russian",
            "SJD in Taxation",
            "Sacred Music",
            "Soil, Water, and Ecosystem Sciences",
            "Sociology",
            "Spanish",
            "Spanish and Portuguese",
            "Sport Management",
            "Stage Management",
            "Statistics",
            "Studio Art",
            "Strings",
            "Sustainability Studies",
            "Sustainability and the Built Environment",
            "Theatre",
            "Theatre Management",
            "Tourism, Hospitality and Event Management",
            "Veternary",
            "Visual Arts in Medicine",
            "Visiting Arists & Scholars",
            "Voice",
            "Wildlife Ecology and Conservation",
            "Women's Studies",
            "Woodwinds",
            "Zoology"
    };
    class dropdown dropdown8(350, 500, 200, 30, options8);

    // create button for featured programs
    sf::RectangleShape featuredButton(sf::Vector2f(170, 40));
    featuredButton.setFillColor(sf::Color(249, 115, 22));
    featuredButton.setOrigin(0, 0);
    featuredButton.setPosition(40, height - 40 - 40);

    // text for featured button
    sf::Text featuredText;
    featuredText.setFont(font);
    featuredText.setString("Featured Programs");
    featuredText.setCharacterSize(20);
    featuredText.setFillColor(sf::Color::Black);
    sf::FloatRect featuredTextRect = featuredText.getLocalBounds();
    featuredText.setOrigin(featuredTextRect.left + featuredTextRect.width/2.0f, featuredTextRect.top + featuredTextRect.height/2.0f);
    featuredText.setPosition(featuredButton.getPosition().x + 170 /2.0f, featuredButton.getPosition().y + 40/2.0f);

    // create button for suggested programs, the ones based on user input
    sf::RectangleShape userInputButton(sf::Vector2f(170, 40));
    userInputButton.setFillColor(sf::Color(249, 115, 22));
    userInputButton.setOrigin(0,0);
    userInputButton.setPosition(width - 170 - 40, height-40-40);

    // text for user input button
    sf::Text userText;
    userText.setFont(font);
    userText.setString("Suggested Programs");
    userText.setCharacterSize(20);
    userText.setFillColor(sf::Color::Black);
    sf::FloatRect userTextRect = userText.getLocalBounds();
    userText.setOrigin(userTextRect.left + userTextRect.width/2.0f, userTextRect.top + userTextRect.height/2.0f);
    userText.setPosition(userInputButton.getPosition().x + 170/2.0f, userInputButton.getPosition().y +40/2.0f);

    // is featured button clicked
    bool featured = false;

    // is suggested button clicked
    bool suggested = false;

    // information-window loop
    while (informationWindow.isOpen()) {
        // create event
        sf::Event event;

        while (informationWindow.pollEvent(event)) {
            // if x button is clicked
            if (event.type == sf::Event::Closed) {
                informationWindow.close();
            }

            // if mouse button is pressed
            if(event.type == sf::Event::MouseButtonPressed){
                // call clicking function for all dropdowns
                sf::Vector2f mousePos = informationWindow.mapPixelToCoords(sf::Mouse::getPosition(informationWindow));
                dropdown.clicking(mousePos);
                dropdown2.clicking(mousePos);
                dropdown3.clicking(mousePos);
                dropdown4.clicking(mousePos);
                dropdown5.clicking(mousePos);
                dropdown6.clicking(mousePos);
                dropdown7.clicking(mousePos);
                dropdown8.clicking(mousePos);
            }

            // if mouse button released
            if(event.type == sf::Event::MouseButtonReleased){
                // call release dragging for all dropdowns
                dropdown.releaseDragging();
                dropdown2.releaseDragging();
                dropdown3.releaseDragging();
                dropdown4.releaseDragging();
                dropdown5.releaseDragging();
                dropdown6.releaseDragging();
                dropdown7.releaseDragging();
                dropdown8.releaseDragging();
            }

            // if mouse is moved
            if(event.type == sf::Event::MouseMoved){
                // call mouse dragging for all dropdowns
                sf::Vector2f mousePos = informationWindow.mapPixelToCoords(sf::Mouse::getPosition(informationWindow));
                dropdown.mouseDragging(mousePos);
                dropdown2.mouseDragging(mousePos);
                dropdown3.mouseDragging(mousePos);
                dropdown4.mouseDragging(mousePos);
                dropdown5.mouseDragging(mousePos);
                dropdown6.mouseDragging(mousePos);
                dropdown7.mouseDragging(mousePos);
                dropdown8.mouseDragging(mousePos);
            }

            // if mouse button is pressed
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                    // if featured button is clicked, close information window and set featured = true
                    if (featuredButton.getGlobalBounds().contains(mousePos)) {
                        informationWindow.close();
                        featured = true;
                    }

                    // if suggested button is clicked, close information window and set suggested = true
                    if (userInputButton.getGlobalBounds().contains(mousePos)) {
                        informationWindow.close();
                        suggested = true;
                    }
                }
            }
        }

        // draw everything
        informationWindow.clear(sf::Color(30, 58, 138));
        informationWindow.draw(informationText);
        informationWindow.draw(college);
        informationWindow.draw(region);
        informationWindow.draw(term);
        informationWindow.draw(gpa);
        informationWindow.draw(country);
        informationWindow.draw(major);
        informationWindow.draw(cost);
        informationWindow.draw(credit);
        informationWindow.draw(featuredButton);
        informationWindow.draw(featuredText);
        informationWindow.draw(userInputButton);
        informationWindow.draw(userText);
        dropdown.draw(informationWindow);
        dropdown2.draw(informationWindow);
        dropdown3.draw(informationWindow);
        dropdown4.draw(informationWindow);
        dropdown5.draw(informationWindow);
        dropdown6.draw(informationWindow);
        dropdown7.draw(informationWindow);
        dropdown8.draw(informationWindow);
        informationWindow.display();
    }

    // if featured is true, open the featured window
    if(featured){
        // render featured-window
        sf::RenderWindow featuredWindow(sf::VideoMode(width, height), "Featured Programs");

        // set up featured text
        sf::Text featuredTitle;
        featuredTitle.setFont(font);
        featuredTitle.setString("Explore Featured Study Abroad Programs!");
        featuredTitle.setCharacterSize(40);
        featuredTitle.setFillColor(sf::Color::White);
        sf::FloatRect featuredTitleRect = featuredTitle.getLocalBounds();
        featuredTitle.setOrigin(featuredTitleRect.left + featuredTitleRect.width / 2.0f,
                                (featuredTitleRect.top + featuredTitleRect.height / 2.0f)+300);
        featuredTitle.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f));

        // Featured program entries
        studyAbroadProgram feature1 = programs[0];
        studyAbroadProgram feature2 = programs[0];
        studyAbroadProgram feature3 = programs[0];
        studyAbroadProgram feature4 = programs[0];
        studyAbroadProgram feature5 = programs[0];
        studyAbroadProgram feature6 = programs[0];
        studyAbroadProgram feature7 = programs[0];
        studyAbroadProgram feature8 = programs[0];
        studyAbroadProgram feature9 = programs[0];
        studyAbroadProgram feature10 = programs[0];

        // find the featured programs, programs selected by our sapa member!
        for(auto p : programs){
            if(p.name == "UF in Dublin - AI Hackathon"){
                feature1 = p;
            }
            if(p.name == "UF in England - Inventors Nurses Farmers and Spies: The Curious History of Statistics"){
                feature2 = p;
            }
            if(p.name == "UF in Florence - Global Perspectives"){
                feature3 = p;
            }
            if(p.name == "UF in Japan - Cross-Cultural Engineering and Design"){
                feature4 = p;
            }
            if(p.name == "UF in Lille - Engineering and Arts in France"){
                feature5 = p;
            }
            if(p.name == "UF in London - Technological Applications for Disability Access"){
                feature6 = p;
            }
            if(p.name == "UF in Panama - Impacts on the Panama Canal"){
                feature7 = p;
            }
            if(p.name == "UF in Singapore - Internship and Global Culture"){
                feature8 = p;
            }
            if(p.name == "UF in South Korea - Culture and Global Design Thinking in Engineering"){
                feature9 = p;
            }
            if(p.name == "UF in South Korea - Global Internship & Culture"){
                feature10 = p;
            }
        }

        // set up featured program 1
        sf::Text program1;
        program1.setFont(font);
        program1.setString(feature1.name);
        program1.setCharacterSize(28);
        program1.setFillColor(sf::Color::White);

        // set up featured program 2
        sf::Text program2;
        program2.setFont(font);
        program2.setString(feature2.name);
        program2.setCharacterSize(28);
        program2.setFillColor(sf::Color::White);

        // set up featured program 3
        sf::Text program3;
        program3.setFont(font);
        program3.setString(feature3.name);
        program3.setCharacterSize(28);
        program3.setFillColor(sf::Color::White);

        // set up featured program 4
        sf::Text program4;
        program4.setFont(font);
        program4.setString(feature4.name);
        program4.setCharacterSize(28);
        program4.setFillColor(sf::Color::White);

        // set up featured program 5
        sf::Text program5;
        program5.setFont(font);
        program5.setString(feature5.name);
        program5.setCharacterSize(28);
        program5.setFillColor(sf::Color::White);

        // set up featured program 6
        sf::Text program6;
        program6.setFont(font);
        program6.setString(feature6.name);
        program6.setCharacterSize(28);
        program6.setFillColor(sf::Color::White);

        // set up featured program 7
        sf::Text program7;
        program7.setFont(font);
        program7.setString(feature7.name);
        program7.setCharacterSize(28);
        program7.setFillColor(sf::Color::White);

        // set up featured program 8
        sf::Text program8;
        program8.setFont(font);
        program8.setString(feature8.name);
        program8.setCharacterSize(28);
        program8.setFillColor(sf::Color::White);

        // set up featured program 9
        sf::Text program9;
        program9.setFont(font);
        program9.setString(feature9.name);
        program9.setCharacterSize(28);
        program9.setFillColor(sf::Color::White);

        // set up featured program 10
        sf::Text program10;
        program10.setFont(font);
        program10.setString(feature10.name);
        program10.setCharacterSize(28);
        program10.setFillColor(sf::Color::White);

        // Positioning below the title
        float baseY = featuredTitle.getPosition().y -230;
        float spacing = 60;
        program1.setPosition(width / 2.0f, baseY);
        program2.setPosition(width / 2.0f, baseY + spacing);
        program3.setPosition(width / 2.0f, baseY + spacing * 2);
        program4.setPosition(width / 2.0f, baseY + spacing * 3);
        program5.setPosition(width / 2.0f, baseY + spacing * 4);
        program6.setPosition(width / 2.0f, baseY + spacing * 5);
        program7.setPosition(width / 2.0f, baseY + spacing * 6);
        program8.setPosition(width / 2.0f, baseY + spacing * 7);
        program9.setPosition(width / 2.0f, baseY + spacing * 8);
        program10.setPosition(width / 2.0f, baseY + spacing * 9);

        // spacing for the featured programs
        vector<sf::Text*> texts = {&program1, &program2, &program3, &program4, &program5, &program6, &program7, &program8, &program9, &program10};
        for (int i = 0; i < 10; ++i) {
            sf::FloatRect bounds = texts[i]->getLocalBounds();
            texts[i]->setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        }

        // featured-window loop
        while(featuredWindow.isOpen()){
            // create event
            sf::Event event;

            while (featuredWindow.pollEvent(event)) {
                // if x button is clicked, close featured window
                if (event.type == sf::Event::Closed) {
                    featuredWindow.close();
                }
            }

            // draw everything
            featuredWindow.clear(sf::Color(30, 58, 138));
            featuredWindow.draw(featuredTitle);
            featuredWindow.draw(program1);
            featuredWindow.draw(program2);
            featuredWindow.draw(program3);
            featuredWindow.draw(program4);
            featuredWindow.draw(program5);
            featuredWindow.draw(program6);
            featuredWindow.draw(program7);
            featuredWindow.draw(program8);
            featuredWindow.draw(program9);
            featuredWindow.draw(program10);
            featuredWindow.display();
        }
    }

    // if suggested is true, open suggested window
    if(suggested){
        string input;
        cout << "Which type of sorting do you want to use? Type 1 for quick sort or type 2 for heap sort." << endl;
        cin >> input;
        while(input != "1" && input != "2"){
            cout << "Please enter 1 or 2!" << endl;
            cin >> input;
        }
        // get college information (based on user input)
        sf::Text text = dropdown.getSelectedText();
        string colleges = text.getString().toAnsiString();

        // get region information (based on user input)
        sf::Text text2 = dropdown2.getSelectedText();
        string regions = text2.getString().toAnsiString();

        // get term information (based on user input)
        sf::Text text3 = dropdown3.getSelectedText();
        string terms = text3.getString().toAnsiString();

        // get gpa information (based on user input)
        sf::Text text4 = dropdown4.getSelectedText();
        string gpas = text4.getString().toAnsiString();

        // get country information (based on user input)
        sf::Text text5 = dropdown5.getSelectedText();
        string countries = text5.getString().toAnsiString();

        // get credit information (based on user input)
        sf::Text text6 = dropdown6.getSelectedText();
        string credits = text6.getString().toAnsiString();

        // get cost information (based on user input)
        sf::Text text7 = dropdown7.getSelectedText();
        string costs = text7.getString().toAnsiString();

        // get major information (based on user input)
        sf::Text text8 = dropdown8.getSelectedText();
        string majors = text8.getString().toAnsiString();

        // sorts by cost, filters by cost, filters by country, filters by gpa
        int maxCost = stoi(costs);
        string temp_gpa = gpas;

        // create vector for filtered programs
        vector<studyAbroadProgram> filtered;

        // if they choose quick sort, do quick sort
        if(input == "1"){
            filtered = filteredProgramsByCost(programs, maxCost);
        }
        // if they choose heap sort, do heap sort
        else{
            filtered = heapSort(programs, maxCost);
        }

        // filter by country and gpa
        filtered = programsByCountry(filtered, countries);
        filtered = programsByGPA(filtered, temp_gpa);

        // render suggested window
        sf::RenderWindow suggestedWindow(sf::VideoMode(width, height), "Suggested Programs");

        // create globe texture, uses globe.png
        sf::Texture globe;
        if (!globe.loadFromFile("globe.png")){
            cerr <<"Failed to load image." << endl;
        }
        sf::Sprite globeSprite;
        globeSprite.setTexture(globe);
        globeSprite.setScale(1.5f, 1.5f);
        sf::FloatRect globeRect = globeSprite.getLocalBounds();
        globeSprite.setOrigin(globeRect.width / 2.0f, globeRect.height / 2.0f);
        globeSprite.setPosition(sf::Vector2f(600, 800));

        // create suggested title text
        sf::Text suggestedTitle;
        suggestedTitle.setFont(font);
        suggestedTitle.setString("Explore Suggested Study Abroad Programs!");
        suggestedTitle.setCharacterSize(40);
        suggestedTitle.setFillColor(sf::Color::White);
        sf::FloatRect suggestedTitleRect = suggestedTitle.getLocalBounds();
        suggestedTitle.setOrigin(suggestedTitleRect.left + suggestedTitleRect.width / 2.0f,
                                (suggestedTitleRect.top + suggestedTitleRect.height / 2.0f)+300);
        suggestedTitle.setPosition(sf::Vector2f(width / 2.0f, height / 2.0f));

        // this sets up how we want to string for each program to appear
        vector<string> programs = {};
        for (auto& p : filtered) {
            programs.push_back(p.name + ":\tCost: $" + p.cost + "\tCountry: " + p.country + "\tCity: " + p.city
            + "\n\tRegion: " + p.region + "\tTerm: " + p.term + "\tGPA Requirement: " + p.gpa
            + "\n\tAdvisor: " + p.advisor + "\tCollege: " + p.college + "\tCredits: " + p.credits
            + "\n\tDid a SAPA complete the program? " + p.sapa);
        }

        // create dropdown for suggested programs
        class dropdown dropdownProgram(25, 150, 1150, 100, programs);

        // suggested window loop
        while(suggestedWindow.isOpen()){
            // create event
            sf::Event event;

            while (suggestedWindow.pollEvent(event)) {
                // if x button clicked, close suggested window
                if (event.type == sf::Event::Closed) {
                    suggestedWindow.close();
                }

                // if mouse button pressed
                if(event.type == sf::Event::MouseButtonPressed){
                    // call clicking on the dropdown
                    sf::Vector2f mousePos = suggestedWindow.mapPixelToCoords(sf::Mouse::getPosition(suggestedWindow));
                    dropdownProgram.clicking(mousePos);
                }

                // if mouse button released
                if(event.type == sf::Event::MouseButtonReleased){
                    // call release dragging on the dropdown
                    dropdownProgram.releaseDragging();
                }

                // if mouse moved
                if(event.type == sf::Event::MouseMoved){
                    // call mouse dragging on the dropdown
                    sf::Vector2f mousePos = suggestedWindow.mapPixelToCoords(sf::Mouse::getPosition(suggestedWindow));
                    dropdownProgram.mouseDragging(mousePos);
                }
            }

            // draw everything
            suggestedWindow.clear(sf::Color(30, 58, 138));
            suggestedWindow.draw(globeSprite);
            suggestedWindow.draw(suggestedTitle);
            dropdownProgram.draw(suggestedWindow);
            suggestedWindow.display();
        }
    }

    return 0;
}