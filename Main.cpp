#include <iostream>
#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <string>
#include "Button.h"


const int floor_height = 90, elevator_height = floor_height, floor_width = 100, elevator_width = floor_width;

const int elevator_speed = 200;    // piksele na sekundy
const int number_of_floors = 7;           
const int passenger_mass = 70;   // waga w kg
const int max_passengers = 8;    


sf::RectangleShape create_floor(int height);
sf::RectangleShape create_elevator();
sf::Text print_text(const std::string& content, const sf::Font& font, unsigned int size, const sf::Color& color, float x, float y);


int main()
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Winda");

	sf::Texture backgroundImage;
	if (!backgroundImage.loadFromFile("szary.jpg"))
	{
		std::cerr << "Nie można załadować obrazu tla." << std::endl;
		return -1;
	}

	sf::Font font;
	if (!font.loadFromFile("ARIAL.TTF"))
	{
		return -2;
	}

	sf::Sprite backgroundSprite(backgroundImage);

	sf::RectangleShape elevator = create_elevator();

	std::vector<sf::RectangleShape> floors_to_draw;
	for (int i = number_of_floors; i >= 0; --i)
	{
		sf::RectangleShape floor = create_floor(i * floor_height);
		floors_to_draw.push_back(floor);
	}

	std::queue<int> floor_queue;//kolejka pieter, na ktorych winda ma sie zatrzymac
	std::queue<int> passenger_queue; //kolejka pasazerow jadacych
	std::queue<int> wait_queue;//kolejka przechowujaca info na ktorym pietrze czeka pasazer
	std::queue<int> destination_floor_queue; //kolejka przechowujaca info na ktore pietro chce pojechac pasazer
	std::queue<int> wait_queue_temp;
	std::queue<int> destination_queue_temp;
	std::queue<int> last_passenger_queue;//zbiera informacje gdzie beda wsiadac pasazerowie
	std::vector<int> target_floors;
	floor_queue.push(7); // Dodajemy parter do kolejki, aby winda blednie nie miala jednego pasazera
	int current_floor = 7, total_weight = 0;
	float destination_y_axis = current_floor * floor_height;


	sf::Text weight_text = print_text("Total Weight: 0 kg", font, 30, sf::Color::Black, 300, 10);
	
	Button myButton1(font, "Add Passenger", sf::Vector2f(700, 200), sf::Vector2f(125, 40));
	Button myButton2(font, "Enter", sf::Vector2f(700, 250), sf::Vector2f(125, 40));

	Button myButton3(font, "Ground", sf::Vector2f(940, 550), sf::Vector2f(65, 40));
	Button myButton4(font, "1", sf::Vector2f(950, 505), sf::Vector2f(40, 40));
	Button myButton5(font, "2", sf::Vector2f(950, 460), sf::Vector2f(40, 40));
	Button myButton6(font, "3", sf::Vector2f(950, 415), sf::Vector2f(40, 40));
	Button myButton7(font, "4", sf::Vector2f(950, 370), sf::Vector2f(40, 40));
	Button myButton8(font, "5", sf::Vector2f(950, 325), sf::Vector2f(40, 40));
	Button myButton9(font, "6", sf::Vector2f(950, 280), sf::Vector2f(40, 40));
	Button myButton10(font, "7", sf::Vector2f(950, 235), sf::Vector2f(40, 40));

	
	sf::Text passenger_counter("", font, 55); // Tekst do wyświetlania aktualnej liczby pasażerów
	passenger_counter.setFillColor(sf::Color::White);
	passenger_counter.setPosition(elevator_width/ 2 - 15, elevator_height / 2 - 20);
	passenger_counter.setOrigin(passenger_counter.getLocalBounds().width / 2, passenger_counter.getLocalBounds().height / 2); //Ustawienie współrzędnych punktu odniesienia tekstu na jego środek 

	sf::Text instruction_text = 
		print_text("INSTRUCTIONS\n\n"
		"1. Press 'Add passenger' button \n"
		"2. Select the floor\n"
		"3. Select the destination\n"
		"4. After selecting all passengers, press ENTER\n"
		"5. Enjoy the simulation", font, 20, sf::Color::White, 700, 10);
	instruction_text.setOutlineThickness(2); // Ustaw grubość konturu tekstu na 2 piksele
	instruction_text.setOutlineColor(sf::Color::Black);


	sf::Text passenger_entering_text = print_text("Starting floor", font, 52, sf::Color::Blue, window.getSize().x / 2, window.getSize().y / 2);
	passenger_entering_text.setOrigin(passenger_entering_text.getLocalBounds().width / 2, passenger_entering_text.getLocalBounds().height / 2);

	sf::Text passenger_destination_text = print_text("Destination floor", font, 52, sf::Color::Blue, window.getSize().x / 2, window.getSize().y / 2);
	passenger_destination_text.setOrigin(passenger_destination_text.getLocalBounds().width / 2, passenger_destination_text.getLocalBounds().height / 2);

	sf::Clock clock;
	sf::Clock waiting_at_floor_clock; // odmierzaniee czasu stania windy na danym pietrze
	float delta_time = 0;

	bool passenger_entering = 0, stopping_at_floor = 0, is_paused = 0;

	int direction = 1; //w dol
	int expected_passengers = 1; //ilu bedzie jechac w windzie
	int currently_in_elevator = 1;// ilu juz wsiadlo do windy


	while (window.isOpen())
	{
		delta_time = clock.restart().asSeconds(); //zliczanie czasu jaki uplynal od momentu restartu zegara
		sf::Event event;
		while (window.pollEvent(event))
		{
			myButton1.handleEvent(event); 
			myButton2.handleEvent(event);
			myButton3.handleEvent(event);
			myButton4.handleEvent(event);
			myButton5.handleEvent(event);
			myButton6.handleEvent(event);
			myButton7.handleEvent(event);
			myButton8.handleEvent(event);
			myButton9.handleEvent(event);
			myButton10.handleEvent(event);


			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			{
				sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

				Button* buttons[] = { &myButton3, &myButton4, &myButton5, &myButton6, &myButton7, &myButton8, &myButton9, &myButton10 };
				int selectedFloorIndices[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

				if (myButton1.contains(mousePos)) //add passenger button
				{
					is_paused = true;
					passenger_entering = false;
				}
				else if (myButton2.contains(mousePos)) //enter button
				{
					if (!passenger_entering)
					{
						is_paused = false;
					}
				}
				else if (is_paused) // wcisnelismy add passenger button
				{
					for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); ++i)
					{
						if (buttons[i]->contains(mousePos))
						{
							int selected_floor = selectedFloorIndices[i];
							if (!passenger_entering)// tutaj wybieramy na ktorym pietrze wsiadaja
							{
								wait_queue.push(selected_floor);
								last_passenger_queue.push(selected_floor);
								passenger_entering = true;
							}
							else //tutaj klikamy destynacje
							{
								destination_floor_queue.push(selected_floor);
								passenger_entering = false;
							}
							break;
						}
					}
				}
			}
		}

		if (!is_paused) // kliknelismy enter
		{
			while (!wait_queue.empty() && expected_passengers < max_passengers) 
			{
				passenger_queue.push(wait_queue.front());    // wejscie pasazera do windy
				wait_queue.pop();// wyrzucenie pasazera z kolejki oczekujacych na jakims pietrze
				expected_passengers++;
			}
			while (!passenger_queue.empty()) 
			{
				if (passenger_queue.front() == current_floor) //sytuacja w ktorej jest wiecej niz 1 pasazer wsiadajacy na tym samym pietrze
				{
					passenger_queue.pop();
					currently_in_elevator++;
				}
				else 
				{
					//pasażerowie, którzy nie weszli do windy wcześniej,czekaja na swoich pietrach i wsiada pozniej
					wait_queue_temp.push(passenger_queue.front());
					passenger_queue.pop();
				}
			}

			while (!wait_queue_temp.empty()) 
			{
				passenger_queue.push(wait_queue_temp.front());
				wait_queue_temp.pop();
			}
			while (!last_passenger_queue.empty()) 
			{
				if (last_passenger_queue.front() == current_floor ) // dodanie pietra do kolejki dopiero kiedy pasazer wsiadzie
				{        
					floor_queue.push(destination_floor_queue.front());
					destination_floor_queue.pop();
					last_passenger_queue.pop();
				}
				else 
				{
					wait_queue_temp.push(last_passenger_queue.front());//wejdzie do kolejki pozniej
					last_passenger_queue.pop();
					destination_queue_temp.push(destination_floor_queue.front());//wysiadzie na tym pietrze pozniej
					destination_floor_queue.pop();
				}
			}
			while (!wait_queue_temp.empty()) 
			{
				last_passenger_queue.push(wait_queue_temp.front());
				wait_queue_temp.pop();
			}
			while (!destination_queue_temp.empty())
			{
				destination_floor_queue.push(destination_queue_temp.front());
				destination_queue_temp.pop();
			}

			float distance = std::abs(elevator.getPosition().y - destination_y_axis);
			if (distance > 1) // 1 = 1 piksel
			{
				if (destination_y_axis < elevator.getPosition().y)
					direction = -1;
				else
					direction = 1;

				float movement = direction * elevator_speed * delta_time;

				elevator.move(0, movement);
				passenger_counter.move(0, movement);
				waiting_at_floor_clock.restart();
			}
			else
			{
				total_weight = abs(currently_in_elevator * passenger_mass);

				if ((!floor_queue.empty() or !passenger_queue.empty()) && !stopping_at_floor)
				{
					int next_floor = current_floor;
					if (passenger_queue.empty())
					{
						next_floor = floor_queue.front();//nie ma pasazerow w windzie, wsiada pasazer z innego pietra
						floor_queue.pop();
						expected_passengers--;
						currently_in_elevator--;
					}
					else if (floor_queue.empty())
					{
						next_floor = passenger_queue.front();
						passenger_queue.pop();
						currently_in_elevator++;
					}  
					else if (abs(current_floor - floor_queue.front()) > abs(current_floor - passenger_queue.front()) && ((current_floor - passenger_queue.front()) * (current_floor - floor_queue.front())) >= 0)
					{
						//winda jedzie tam gdzie ma blizej + zatrzyma sie jesli jest po drodze do aktualnego celu
						next_floor = passenger_queue.front();
						passenger_queue.pop();
						currently_in_elevator++;
					}    
					else
					{
						next_floor = floor_queue.front();
						floor_queue.pop();
						expected_passengers--;
						currently_in_elevator--;
					}
					target_floors.push_back(next_floor);
					stopping_at_floor = 1;
					destination_y_axis = next_floor * floor_height;
					current_floor = next_floor;
					while (!floor_queue.empty()) 
					{
						if (floor_queue.front() == current_floor) //dodanie do kolejki destynacji kazdego pasazera
						{
							floor_queue.pop();
							expected_passengers--;
							currently_in_elevator--;
						}
						else 
						{
							wait_queue_temp.push(floor_queue.front());
							floor_queue.pop();
						}
					}
					while (!wait_queue_temp.empty()) 
					{
						floor_queue.push(wait_queue_temp.front());
						wait_queue_temp.pop();
					}

				}
				else if (!target_floors.empty())
				{
					if (waiting_at_floor_clock.getElapsedTime().asSeconds() >= 1)
					{
						target_floors.erase(target_floors.begin());//usuwamy pietro, na ktore dojechalismy
						stopping_at_floor = 0;
						waiting_at_floor_clock.restart();
					}
				}
				else if (waiting_at_floor_clock.getElapsedTime().asSeconds() >= 5)
				{
					target_floors.push_back(number_of_floors);//jedziemy na parter
					stopping_at_floor = 1;
					destination_y_axis = (number_of_floors) * floor_height;
					current_floor = (number_of_floors);
				}
			}
			// tworzenie tekstu liczby pasazerow i ich masy
			weight_text.setString("Total Weight: " + std::to_string(int(total_weight)) + " kg");
			passenger_counter.setString(std::to_string(int(total_weight / 70)));
		}
		
		window.clear(sf::Color::White);
		
		window.draw(backgroundSprite); //wazne, aby narysowac tlo na samym poczatku
		// rysowanie kazdego pietra
		for (const auto& floor : floors_to_draw)
			window.draw(floor);

		window.draw(elevator); //winda
		window.draw(weight_text); // masa pasazerow
		window.draw(passenger_counter); // liczba pasazerow
		window.draw(instruction_text);
		
		if (is_paused && !passenger_entering) //printowanie napisu "starting floor"
			window.draw(passenger_entering_text);

		if (is_paused && passenger_entering) // //printowanie napisu "destination floor"
			window.draw(passenger_destination_text);


		myButton1.render(window);
		myButton2.render(window);
		myButton3.render(window);
		myButton4.render(window);
		myButton5.render(window);
		myButton6.render(window);
		myButton7.render(window);
		myButton8.render(window);
		myButton9.render(window);
		myButton10.render(window);
		
		window.display();
	}

	return 0;
}

sf::RectangleShape create_floor(int height)
{
	sf::RectangleShape floor(sf::Vector2f(floor_width, floor_height));
	floor.setPosition(0, height);
	floor.setFillColor(sf::Color::White);
	floor.setOutlineThickness(2);
	floor.setOutlineColor(sf::Color::Black);
	return floor;
}

sf::RectangleShape create_elevator()
{
	sf::RectangleShape elevator(sf::Vector2f(elevator_width, elevator_height));
	elevator.setFillColor(sf::Color::Red);
	elevator.setOutlineThickness(4);
	elevator.setOutlineColor(sf::Color::Black);
	return elevator;
}

sf::Text print_text(const std::string& content, const sf::Font& font, unsigned int size, const sf::Color& color, float x, float y)
{
	sf::Text text(content, font, size);
	text.setFillColor(color);
	text.setPosition(x, y);
	return text;
}