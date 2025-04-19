#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
using namespace std;

#ifndef FINALPROJECTDSA_DROPDOWN_H
#define FINALPROJECTDSA_DROPDOWN_H

// creates the dropdowns used on the information window and the suggested window
class dropdown{
private:
    // all the sfml stuff for the text and box
    sf:: Font font;
    sf::RectangleShape box;
    sf::Text selectedText;
    vector<sf::RectangleShape> options;
    vector<sf::Text> textOptions;

    // is the dropdown open?
    bool isOpen = false;

    // tell us which index is selected
    int selectedIndex = 0;

    // all the sfml stuff for the scrollbar itself
    sf::RectangleShape track;
    sf::RectangleShape scrollBar;


    float scrollWidth = 10.0f;
    int scrollOffset = 0;

    // want to show five options at a time
    int visibleCount = 5;

    // are we actively dragging the scrollbar?
    bool isDragging = false;

    // how far did we drag the scrollbar?
    float dragOffset = 0;



public:
    // returns the text that is selected, necessary to get the information the users inputs
    sf::Text getSelectedText(){
        return selectedText;
    }

    // gets the bounds of the box
    sf::FloatRect getBound(){
        return box.getGlobalBounds();
    }

    // checks to see if the dropdown is open
    bool getIsOpen(){
        return isOpen;
    }

    // constructor for the dropdown class
    dropdown(float x, float y, float width, float height, vector<string> options){
        // same font as in main
        font.loadFromFile("times.ttf");

        // sets the boxes position to what the main file says
        box.setPosition(x, y);

        // sets the boxes size to what the main file says
        box.setSize({width, height});

        // same color as used in main
        box.setFillColor(sf::Color(249, 115, 22));

        // use the font, set the character size,
        // text color is black, selected text starts at
        // index 0, sets position within the box
        selectedText.setFont(font);
        selectedText.setCharacterSize(18);
        selectedText.setFillColor(sf::Color::Black);
        selectedText.setString(options[0]);
        selectedText.setPosition(x+5, y+5);

        // loop through options, which are the texts that goes within the dropdown
        for(size_t i = 0; i < options.size(); i++){
            // create rectangle, set position within dropdown, same color as main
            sf::RectangleShape box({width, height});
            box.setPosition(x, y + (i+1)*height);
            box.setFillColor(sf::Color(249, 115, 22));

            // create the text using the font,
            // set the text to be each option,
            // set character size, color, and
            // position within the box
            sf::Text text;
            text.setFont(font);
            text.setString(options[i]);
            text.setCharacterSize(18);
            text.setFillColor(sf::Color::Black);
            text.setPosition(x+5, y+(i+1)*height+5);

            // add box to options
            this->options.push_back(box);
            // add text to textOptions
            textOptions.push_back(text);
        }

        // setting up the track for the dropdown (the box that holds the thing you actually use to scroll)
        track.setSize(sf::Vector2f(scrollWidth, box.getSize().y * 5));
        track.setFillColor(sf::Color(200, 200, 200));
        track.setPosition(box.getPosition().x+box.getSize().x-scrollWidth, box.getPosition().y+box.getSize().y);

        // set up the scrollbar, the thing you actually drag
        scrollBar.setSize(sf::Vector2f(scrollWidth, 50));
        scrollBar.setFillColor(sf::Color(100, 100, 100));
    }

    // called within main to actually draw the dropdown and scrollbar
    void draw(sf::RenderWindow& window){
        // draw the box and selectedText, which starts at index 0
        window.draw(box);
        window.draw(selectedText);

        // if the dropdown is open
        if(isOpen){
            // get the height of the dropdown (does not include the original box size)
            float heightTotal = box.getSize().y * visibleCount;

            // set the height of the scrollbar
            float heightScrollBar = heightTotal * ((float)visibleCount/options.size());

            // get the range: the total height - height of the scrollbar
            float range = heightTotal - heightScrollBar;
            float posY = track.getPosition().y + (scrollOffset / float(options.size() - visibleCount)) * range;

            // setting track size and set position
            track.setSize(sf::Vector2f(scrollWidth, heightTotal));
            track.setPosition(box.getPosition().x + box.getSize().x - scrollWidth, box.getPosition().y + box.getSize().y);

            // setting scrollbar size and position
            scrollBar.setSize(sf::Vector2f(scrollWidth, heightScrollBar));
            scrollBar.setPosition(track.getPosition().x, posY);

            // get the end of the scrolling, we need to know when we cannot scroll any further
            int end = min(scrollOffset + visibleCount, (int)options.size());

            // loop starting at scrollOffset and going to end
            for(int i = scrollOffset; i < end; i++){
                // find offset
                float offsetY = (i-scrollOffset + 1) * box.getSize().y;

                // set position of options and textOptions
                options[i].setPosition(box.getPosition().x, box.getPosition().y +offsetY);
                textOptions[i].setPosition(box.getPosition().x+5, box.getPosition().y+offsetY+5);

                // draw the ones that are now showing
                window.draw(options[i]);
                window.draw(textOptions[i]);
            }

            // draw the track and scrollbar
            window.draw(track);
            window.draw(scrollBar);
        }
    }

    // handles the clicking of the dropdown menu
    bool clicking(sf::Vector2f mousePos){
        // if box is clicked, change isOpen to either true or false
        if(box.getGlobalBounds().contains(mousePos)){
            isOpen = !isOpen;
            return true;
        }

        // if dropdown is open
        if(isOpen){
            // if we are scrolling using the scrollbar
            if(scrollBar.getGlobalBounds().contains(mousePos)){
                // set isDragging equal to true
                isDragging = true;

                //determine drag offset
                dragOffset = mousePos.y - scrollBar.getPosition().y;
                return true;
            }

            // loop, starting at scroll offset going until visible count and size of options
            for(size_t i = scrollOffset; i < scrollOffset + visibleCount && i < options.size(); i++){
                // if user clicks one of the boxes
                if(options[i].getGlobalBounds().contains(mousePos)){
                    // change selected index and selected text
                    selectedIndex = i;
                    selectedText.setString(textOptions[i].getString());

                    // close the dropdown
                    isOpen = false;
                    return true;
                }
            }
            isOpen = false;
        }
        return false;
    }

    // sets isDragging = false
    void releaseDragging(){
        isDragging = false;
    }

    // handles the dragging
    void mouseDragging(sf::Vector2f mousePos){
        // if we aren't dragging or the dropdown is not open
        if(!isDragging || !isOpen){
            return;
        }

        // get total height, scrollbar height, and the range
        float totalHeight = box.getSize().y * visibleCount;
        float scrollbarHeight = scrollBar.getSize().y;
        float range = totalHeight - scrollbarHeight;

        // calculate y position of scrollbar
        float yNew = mousePos.y - dragOffset;

        // make sure the y position does not exceed the track
        yNew = max(track.getPosition().y, min(yNew, track.getPosition().y + range));

        // update scrollbar position
        scrollBar.setPosition(scrollBar.getPosition().x, yNew);

        // get scroll percentage, how far it has moved based on the full range
        float per = (yNew - track.getPosition().y) / range;

        // determine the scroll offset
        scrollOffset = per * (options.size() - visibleCount);
    }
};
#endif
