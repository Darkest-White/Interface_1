#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>
using namespace sf;
using namespace std;
using namespace chrono;

void random_rect(float radius, RectangleShape* rectangle) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.f, 2.f * 3.14);

	float angle = dis(gen);
	float x = radius * cos(angle);
	float y = radius * sin(angle);

	rectangle->setPosition(abs(x), abs(y));
}

int main()
{
	setlocale(LC_ALL, "rus");

	int choice;
	cout << "������� ����� ������������: ";
	cin >> choice;

	RenderWindow window(VideoMode(800, 700), "1lab_HCI", Style::Close);

	auto last_click_time = steady_clock::now();

	Vector2i pos = window.getPosition();
	pos.y += 32;

	bool IsClicked = 0;
	bool IsMoved = 0;

	float radius = 200.f;
	float size = 8.f;
	sf::RectangleShape rectangle(sf::Vector2f(2 * size, size));
	rectangle.setFillColor(Color(128, 128, 128));

	sf::CircleShape circle(radius);
	circle.setPointCount(200);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineThickness(1.f);
	circle.setOutlineColor(sf::Color::Red);
	circle.setPosition(-radius, -radius);

	vector<float> dists = { 1.f, 20.f, 40.f, 60.f, 100.f, 150.f, 200.f, 250.f, 300.f, 350.f };
	vector<float> sizes = { 8.f, 10.f, 12.f, 15.f, 20.f, 30.f , 40.f, 50.f, 70.f, 100.f };

	int cur_dist = 0;
	int cur_size = 0;
	int cur_try = 0;

	int tries = 5;

	vector<float> med_nums;
	vector<vector<float>> med_nums2;
	med_nums.push_back(0.f);
	med_nums2.push_back(*new vector<float>);
	med_nums2.at(0).push_back(0.f);


	if (choice == 3) tries = 3;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed or choice > 3 or choice < 1)
				window.close();
			else if (cur_dist >= dists.size() and (choice == 1 or choice == 3))
				window.close();
			else if (cur_size >= sizes.size() and choice == 2)
				window.close();
			else if (event.type == Event::KeyPressed and event.key.code == Keyboard::Space and not IsClicked and not IsMoved) {
				IsClicked = 1;

				pos = window.getPosition();
				pos.y += 30;
				pos.x += 8;
				Mouse::setPosition(pos);

				if (choice == 1 or choice == 3) {
					radius = dists.at(cur_dist);
					circle.setRadius(radius);
					circle.setPosition(-radius, -radius);
				}
				if (choice == 2 or choice == 3) {
					size = sizes.at(cur_size);
					rectangle.setSize({ size, size });
				}

				random_rect(radius, &rectangle);
			}
			else if (IsClicked and Mouse::getPosition() != pos and not IsMoved) {
				IsMoved = 1;

				last_click_time = steady_clock::now();
			}
			else if (IsMoved == 1 and IsClicked and event.type == Event::MouseButtonPressed and rectangle.getGlobalBounds().contains(Mouse::getPosition(window).x, Mouse::getPosition(window).y))
			{
				auto current_time = steady_clock::now();
				auto time_since_last_click = duration_cast<nanoseconds>(current_time - last_click_time).count();

				cout << "�����: " << static_cast<float>(time_since_last_click) / 1e9 << " ������" << endl;

				last_click_time = current_time;

				if (cur_try < tries - 1) {
					if (choice == 1) med_nums.at(cur_dist) += static_cast<float>(time_since_last_click) / 1e9;
					else if (choice == 2) med_nums.at(cur_size) += static_cast<float>(time_since_last_click) / 1e9;
					else if (choice == 3) med_nums2.at(cur_dist).at(cur_size) += static_cast<float>(time_since_last_click) / 1e9;
					cur_try++;
				}
				else {
					if (choice == 1) {
						med_nums.push_back(0.f);
						med_nums.at(cur_dist) += static_cast<float>(time_since_last_click) / 1e9;
						med_nums.at(cur_dist) /= 5;
						cur_dist++;
					}
					else if (choice == 2) {
						med_nums.push_back(0.f);
						med_nums.at(cur_size) += static_cast<float>(time_since_last_click) / 1e9;
						med_nums.at(cur_size) /= 5;
						cur_size++;
					}
					else if (choice == 3) {
						med_nums2.at(cur_dist).at(cur_size) += static_cast<float>(time_since_last_click) / 1e9;
						med_nums2.at(cur_dist).at(cur_size) /= tries;
						if (cur_size < sizes.size() - 1) {
							cur_size++;
						}
						else {
							cur_size = 0;
							cur_dist++;
							med_nums2.push_back(*new vector<float>);
						}
						med_nums2.at(cur_dist).push_back(0.f);
					}
					cur_try = 0;
				}

				IsClicked = 0;
				IsMoved = 0;
			}
		}
		window.clear(Color::Black);
		if (IsClicked) {
			window.draw(rectangle);
			window.draw(circle);
		}
		window.display();
	}
	if (choice == 1)
	{
		cout << "������� �������� ������� ����� ���������:" << endl;
		for (int i = 0; i < dists.size(); i++)
			cout << "��� �������� " << dists[i] << ": " << med_nums[i] << endl;
	}
	else if (choice == 2)
	{
		cout << "������� �������� ������� ����� ���������:" << endl;
		for (int i = 0; i < sizes.size(); i++)
			cout << "��� �������� " << sizes[i] << ": " << med_nums[i] << endl;

	}
	else if (choice == 3)
	{
		cout << "������� �������� ������� ����� ���������:" << endl;
		for (int i = 0; i < med_nums2.size(); i++)
		{
			for (int j = 0; j < med_nums2[i].size(); j++)
			{
				cout << "��� ���������� " << dists[i] << " � ������� " << sizes[j] << ": " << med_nums2[i][j] << endl;
			}
		}
	}
	ofstream outputFile("average_times.txt", ios::app);

	if (outputFile.is_open())
	{
		outputFile << "����� ������������: " << choice << endl;
		outputFile << "������� �������� ������� ����� ���������:" << endl;

		// ������ ������� �������� � ����
		if (choice == 1)
			for (int i = 0; i < dists.size(); i++)
				outputFile << "��� �������� " << dists[i] << ": " << med_nums[i] << endl;
		else if (choice == 2)
			for (int i = 0; i < sizes.size(); i++)
				outputFile << "��� �������� " << sizes[i] << ": " << med_nums[i] << endl;
		else if (choice == 3)
			for (int i = 0; i < med_nums2.size(); i++)
			{
				for (int j = 0; j < med_nums2[i].size(); j++)
				{
					outputFile << "��� ���������� " << dists[i] << " � ������� " << sizes[j] << ": " << med_nums2[i][j] << endl;
				}
			}

		outputFile.close();

		cout << "������ �� ������������ " << choice << " ���� ��������� � ���� 'average_times.txt'" << endl;
	}
	else
	{
		cerr << "�� ������� ������� ���� ��� ������." << endl;
	}
	return 0;
}