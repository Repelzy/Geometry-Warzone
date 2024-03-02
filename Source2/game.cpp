#include "Game.h"
#include <fstream>
#include <iostream>
#include <cstdlib> // for srand
#include <ctime>   // for time
#define _USE_MATH_DEFINES
#include <math.h>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::run()
{
	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			sLifeSpan();
			sSpawner();
			sMovement();
			sCollision();
		}
		sUserInput();
		sRender();
		//Increment the current frame
		++m_currentFrame;
	}
}

// initialize the game using config file

void Game::init(const std::string& configPath) {
	// Seed the random number generator
	srand(static_cast<unsigned int>(time(0)));

	// Read in the config file 
	std::ifstream input(configPath);

	if (!input.is_open()) {
		std::cout << "Failed to open: " << configPath << '\n';
		exit(-1); // Exiting program if file opening fails
	}

	std::string id{};
	while (input >> id)
	{
		if (id == "Window")
		{
			unsigned int width{};
			input >> width;

			unsigned int height{};
			input >> height;

			int frameLimit{};
			input >> frameLimit;

			int isFullScreen{};
			input >> isFullScreen;
			bool fullScreen = (isFullScreen == 0) ? false : true;

			if (!fullScreen)
			{
				m_window.create(sf::VideoMode(width, height), "Geometry Wars", sf::Style::Close);
			}
			else
			{
				auto fullscreenMode{ sf::VideoMode::getFullscreenModes() };
				//fullscreenMode[0] is the most compatible mode for fullscreen on this device
				m_window.create(fullscreenMode[0], "Geometry Wars", sf::Style::Fullscreen);
			}
			m_window.setFramerateLimit(frameLimit);
		}
		else if (id == "Font")
		{
			std::string filepath;
			input >> filepath;

			// Load font file
			if (!m_font.loadFromFile(filepath))
			{
				std::cerr << "Failed to load font. Filepath: " << filepath << std::endl;
				// Handle font loading failure (optional)
			}
			else
			{
				// Font loading successful
				m_scoreText.setFont(m_font);

				int fontSize;
				input >> fontSize;
				m_scoreText.setCharacterSize(fontSize);

				int r, g, b;
				input >> r >> g >> b;
				m_scoreText.setFillColor(sf::Color(r, g, b));
			}
		}

 
		// load player info
		else if (id == "Player")
		{
			int shapeRadius{};
			input >> shapeRadius;
			m_playerConfig.SR = shapeRadius;

			int collisionRadius{};
			input >> collisionRadius;
			m_playerConfig.CR = collisionRadius;

			float speed{};
			input >> speed;
			m_playerConfig.S = speed;

			sf::Vector3<sf::Uint16> fillColor{};
			input >> fillColor.x;
			input >> fillColor.y;
			input >> fillColor.z;
			m_playerConfig.FR = fillColor.x;
			m_playerConfig.FG = fillColor.y;
			m_playerConfig.FB = fillColor.z;

			sf::Vector3<sf::Uint16> outlineColor{};
			input >> outlineColor.x;
			input >> outlineColor.y;
			input >> outlineColor.z;
			m_playerConfig.OR = outlineColor.x;
			m_playerConfig.OG = outlineColor.y;
			m_playerConfig.OB = outlineColor.z;

			int outlineThickness{};
			input >> outlineThickness;
			m_playerConfig.OT = outlineThickness;

			int vertices{};
			input >> vertices;
			m_playerConfig.V = vertices;
		}

		// load enemy info
		else if (id == "Enemy")
		{
			int shapeRadius{};
			input >> shapeRadius;
			m_enemyConfig.SR = shapeRadius;

			int collisionRadius{};
			input >> collisionRadius;
			m_enemyConfig.CR = collisionRadius;

			float minSpeed{};
			input >> minSpeed;
			m_enemyConfig.SMIN = minSpeed;

			float maxSpeed{};
			input >> maxSpeed;
			m_enemyConfig.SMAX = maxSpeed;

			sf::Vector3<sf::Uint16> outlineColor{};
			input >> outlineColor.x;
			input >> outlineColor.y;
			input >> outlineColor.z;
			m_enemyConfig.OR = outlineColor.x;
			m_enemyConfig.OG = outlineColor.y;
			m_enemyConfig.OB = outlineColor.z;

			int outlineThickness{};
			input >> outlineThickness;
			m_enemyConfig.OT = outlineThickness;

			int minVertices{};
			input >> minVertices;
			m_enemyConfig.VMIN = minVertices;

			int maxVertices{};
			input >> maxVertices;
			m_enemyConfig.VMAX = maxVertices;

			int smallLifespan{};
			input >> smallLifespan;
			m_enemyConfig.L = smallLifespan;

			int spawnInterval{};
			input >> spawnInterval;
			m_enemyConfig.SI = spawnInterval;
		}
		else if (id == "Bullet")
		{
			int shapeRadius{};
			input >> shapeRadius;
			m_bulletConfig.SR = shapeRadius;

			int collisionRadius{};
			input >> collisionRadius;
			m_bulletConfig.CR = collisionRadius;

			float speed{};
			input >> speed;
			m_bulletConfig.S = speed;

			sf::Vector3<sf::Uint16> fillColor{};
			input >> fillColor.x;
			input >> fillColor.y;
			input >> fillColor.z;
			m_bulletConfig.FR = fillColor.x;
			m_bulletConfig.FG = fillColor.y;
			m_bulletConfig.FB = fillColor.z;

			sf::Vector3<sf::Uint16> outlineColor{};
			input >> outlineColor.x;
			input >> outlineColor.y;
			input >> outlineColor.z;
			m_bulletConfig.OR = outlineColor.x;
			m_bulletConfig.OG = outlineColor.y;
			m_bulletConfig.OB = outlineColor.z;

			int outlineThickness{};
			input >> outlineThickness;
			m_bulletConfig.OT = outlineThickness;

			int shapeVertices{};
			input >> shapeVertices;
			m_bulletConfig.V = shapeVertices;

			int lifespan{};
			input >> lifespan;
			m_bulletConfig.L = lifespan;

			int specialBullet{};
			input >> specialBullet;
			m_bulletConfig.SB = specialBullet;

			int timer{};
			input >> timer;
			m_timer = timer;
		}

		m_scoreText.setPosition(0, 0);
		m_scoreText.setString(std::to_string(m_score));

		spawnPlayer();
	}
}



void Game::setPaused(bool paused)
{
	m_paused = paused;
}



void Game::sMovement()
{   
	// implement player movement
	Vec2 playerV;
	if (m_player->cInput->left) {
		playerV.x -= m_playerConfig.S;
	}
	if (m_player->cInput->right)
	{
		playerV.x += m_playerConfig.S;
	}
	if (m_player->cInput->up)
	{
		playerV.y -= m_playerConfig.S;
	}
	if (m_player->cInput->down)
	{
		playerV.y += m_playerConfig.S;
	}
	for (auto e : m_entities.getEntities()) {
		if (e->tag() == "player") {
			m_player->cTransform->pos += playerV;
			e->cTransform->angle += 2.0f;
			e->cShape->circle.setRotation(e->cTransform->angle);
		}
		else if (e->cTransform)
		{
			//Updates the position of the entities
			e->cTransform->pos += e->cTransform->velocity;
			//Rotates the entity
			e->cTransform->angle += 2.0f;
			e->cShape->circle.setRotation(e->cTransform->angle);
		}
	}
}

void Game::sUserInput() {
	sf::Event event;

	// while there are pending events...
	while (m_window.pollEvent(event)) {
		switch (event.type)
		{
			// window closed
			case sf::Event::Closed: {
				m_running = false;
				break;
			}
			case sf::Event::KeyPressed: {
				if (event.key.code == sf::Keyboard::Escape) {
					m_running = false;
				}
				else if (event.key.code == sf::Keyboard::P) {
					m_paused ? setPaused(false) : setPaused(true);
				}
				else if (event.key.code == sf::Keyboard::A) { 
					m_player->cInput->left = true; 
				}
				else if (event.key.code == sf::Keyboard::D) { 
					m_player->cInput->right = true; 
				}
				else if (event.key.code == sf::Keyboard::W) { 
					m_player->cInput->up = true; 
				}
				else if (event.key.code == sf::Keyboard::S) { 
					m_player->cInput->down = true; 
				}
				break;
			}
			case sf::Event::KeyReleased: {
				if (event.key.code == sf::Keyboard::A) {
					m_player->cInput->left = false;
				}
				else if (event.key.code == sf::Keyboard::D) { 
					m_player->cInput->right = false; 
				}
				else if (event.key.code == sf::Keyboard::W) { 
					m_player->cInput->up = false; 
				}
				else if (event.key.code == sf::Keyboard::S) { 
					m_player->cInput->down = false; 
				}
				break;
			}
			case sf::Event::MouseButtonPressed: {
				if (event.mouseButton.button == sf::Mouse::Left) {
					spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
				}
				if (event.mouseButton.button == sf::Mouse::Right) {
					if (m_currentFrame - m_lastSpecialWeaponTime >= m_timer) {
						spawnSpecialWeapon(m_player);
						m_lastSpecialWeaponTime = m_currentFrame;
					}
				}
				break;
			}
			case sf::Event::MouseButtonReleased:
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					m_player->cInput->leftMouse = false;
				}
				break;
			}
			default:
				break;
		}
	}
}

//Calculate lifespan of the entity after frame
void Game::sLifeSpan() {
	for (auto e : m_entities.getEntities()) {
		if (!e->cLifespan) {
			continue;
		}
		if (e->cLifespan->remaining > 0) {
			--e->cLifespan->remaining;
		}
		if (e->isActive() && e->cLifespan->remaining > 0) {
			float alpha = static_cast<float>(e->cLifespan->remaining) / static_cast<float>(e->cLifespan->total);
			auto& fillC = e->cShape->circle.getFillColor(); //retrieve original color
			sf::Color newColor = { fillC.r, fillC.g, fillC.b, static_cast<sf::Uint8>(255 * alpha) };
			e->cShape->circle.setFillColor(newColor); // Set the fill color using the calculated alpha value

			auto& OutLineColor = e->cShape->circle.getFillColor();
			sf::Color newOutLineColor{ OutLineColor.r,OutLineColor.g,OutLineColor.b, static_cast<sf::Uint8>(255 * alpha) };
			e->cShape->circle.setOutlineColor(newOutLineColor);
		}
		else if (e->cLifespan->remaining <= 0) {
			e->destroy();
		}
	}
}

void Game::sRender()
{
	m_window.clear();

	for (auto e : m_entities.getEntities())
	{
		// Check if entity has cShape and cTransform components
		if (e->cShape && e->cTransform) {
			// Update position of the shape based on cTransform
			e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
			// Rotate the entity by 1.0f every frame
			e->cTransform->angle += 1.0f;

			// Set rotation of the shape
			e->cShape->circle.setRotation(e->cTransform->angle);

			// Draw the rotated shape
			m_window.draw(e->cShape->circle);
		}
	}
	m_window.draw(m_scoreText);

	// Display everything
	m_window.display();
}




// calculate time to spawn enemy so it doesnt overlap
void Game::sSpawner() {
	// Check if it's time to spawn an enemy
	if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI) {
		spawnEnemy();
	}
}



void Game::sCollision()
{
	for (auto& player : m_entities.getEntities("player"))
	{
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			//if there's a collision between player and enemy
			//Destroy the player,//Destroy the enemy,//Respawn the player

			Vec2 diff{ enemy->cTransform->pos.x - player->cTransform->pos.x , enemy->cTransform->pos.y - player->cTransform->pos.y };

			double collisionRadiusSQ{ (player->cCollision->radius + enemy->cCollision->radius) * (player->cCollision->radius + enemy->cCollision->radius) };
			double distSQ{ (diff.x * diff.x) + (diff.y * diff.y) };

			if (distSQ < collisionRadiusSQ)
			{
				//makes sure the player is alive and doesnt spawn 2 players
				if (player->isActive())
				{
					m_score = 0;
					m_scoreText.setString(std::to_string(m_score));

					enemy->destroy();
					player->destroy();
					spawnPlayer();
				}
			}
		}
	}

	//	for (auto& enemy : m_entities.getEntities("smallEnemy"))
	//	{
	//		//if there's a collision between player and small enemy
	//		//Destroy the player,//Destroy the enemy,//Respawn the player

	//		Vec2 diff{ enemy->cTransform->pos.x - player->cTransform->pos.x , enemy->cTransform->pos.y - player->cTransform->pos.y };

	//		double collisionRadiusSQ{ (player->cCollision->radius + enemy->cCollision->radius) * (player->cCollision->radius + enemy->cCollision->radius) };
	//		double distSQ{ (diff.x * diff.x) + (diff.y * diff.y) };

	//		if (distSQ < collisionRadiusSQ)
	//		{
	//			//makes sure the player is alive so it doesnt spawn 2 players
	//			if (player->isActive())
	//			{
	//				m_score /= 2;
	//				m_scoreText.setString(std::to_string(m_score));

	//				player->destroy();
	//				enemy->destroy();
	//				spawnPlayer();
	//			}
	//		}
	//	}
	//}

	for (auto& bullet : m_entities.getEntities("bullet"))
	{
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			//if there's a collision between player and enemy
			//Destroy the player,//Destroy the enemy,//Respawn the player

			if (bullet && enemy && bullet->cTransform && enemy->cTransform && bullet->cCollision && enemy->cCollision && enemy->cScore)
			{
				Vec2 diff = { enemy->cTransform->pos.x - bullet->cTransform->pos.x , enemy->cTransform->pos.y - bullet->cTransform->pos.y };

				double collisionRadiusSQ = (bullet->cCollision->radius + enemy->cCollision->radius) * (bullet->cCollision->radius + enemy->cCollision->radius);
				double distSQ = (diff.x * diff.x) + (diff.y * diff.y);

				if (distSQ < collisionRadiusSQ)
				{
					//Updates the score
					m_score += enemy->cScore->score;
					m_scoreText.setString(std::to_string(m_score));

					spawnSmallEnemies(enemy);
					bullet->destroy();
					enemy->destroy();

					break;
				}
			}
		}

		for (auto& enemy : m_entities.getEntities("smallEnemy"))
		{
			//Updates the score

			Vec2 diff = { enemy->cTransform->pos.x - bullet->cTransform->pos.x , enemy->cTransform->pos.y - bullet->cTransform->pos.y };

			double collisionRadiusSQ{ (bullet->cCollision->radius + enemy->cCollision->radius) * (bullet->cCollision->radius + enemy->cCollision->radius) };
			double distSQ{ (diff.x * diff.x) + (diff.y * diff.y) };

			if (distSQ < collisionRadiusSQ)
			{
				m_score += enemy->cScore->score;
				m_scoreText.setString(std::to_string(m_score));

				bullet->destroy();
				enemy->destroy();

				break;
			}
		}
	}

	//General collision for player

	for (auto& e : m_entities.getEntities("player"))
	{
		//Checks to see if player collided with walls
		if (e->cTransform->pos.x + m_playerConfig.CR > m_window.getSize().x)
		{
			e->cTransform->pos.x -= m_playerConfig.S;
		}
		else if (e->cTransform->pos.x - m_playerConfig.CR < 0)
		{
			e->cTransform->pos.x += m_playerConfig.S;
		}

		if (e->cTransform->pos.y + m_playerConfig.CR > m_window.getSize().y)
		{
			e->cTransform->pos.y -= m_playerConfig.S;
		}
		else if (e->cTransform->pos.y - m_playerConfig.CR < 0)
		{
			e->cTransform->pos.y += m_playerConfig.S;
		}
	}

	//General collision for entities
	for (auto& e : m_entities.getEntities())
	{
		if (/*e->tag() == "bullet" ||*/ e->tag() == "enemy" || e->tag() == "smallEnemy") {
			if (e->cTransform && e->cCollision) {
				if (e->cTransform->pos.x + e->cCollision->radius > m_window.getSize().x) {
					e->cTransform->velocity.x *= -1;
				}
				else if (e->cTransform->pos.x - e->cCollision->radius < 0) {
					e->cTransform->velocity.x *= -1;
				}
				if (e->cTransform->pos.y + e->cCollision->radius > m_window.getSize().y) {
					e->cTransform->velocity.y *= -1;
				}
				else if (e->cTransform->pos.y - e->cCollision->radius < 0) {
					e->cTransform->velocity.y *= -1;
				}
			}
		}
	}
}

// create a spawn setup for the player
void Game::spawnPlayer() {
	// Check if a player entity already exists
	if (m_player) {
		// If so, destroy the existing player entity
		m_player->destroy();
	}

	// Create a new player entity
	auto entity = m_entities.addEntity("player");

	float midx = m_window.getSize().x / 2.0f;
	float midy = m_window.getSize().y / 2.0f;

	entity->cTransform = std::make_shared<CTransform>(Vec2(midx, midy), Vec2(0, 0), 0.0f);
	// Creates the entity's shape
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG,
		m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

	// Give the entity a collision component
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// Add an input component to take the player's input
	entity->cInput = std::make_shared<CInput>();

	// Assign the new player entity to the member variable
	m_player = entity;
}


// create a spawn setup for the enemy
void Game::spawnEnemy() {
    auto e = m_entities.addEntity("enemy");

    // Spawn enemy at random location
    int minRangeX = m_enemyConfig.CR;
    int maxRangeX = m_window.getSize().x - minRangeX;
    int minRangeY = m_enemyConfig.CR;
    int maxRangeY = m_window.getSize().y - minRangeY;

    Vec2 randPos{
        static_cast<float>(minRangeX + (rand() % (maxRangeX - minRangeX + 1))),
        static_cast<float>(minRangeY + (rand() % (maxRangeY - minRangeY + 1)))
    };


    // Generate a random speed within the specified range
    float minSpeed = m_enemyConfig.SMIN;
    float maxSpeed = m_enemyConfig.SMAX;
    float speedRange = maxSpeed - minSpeed + 1;
    float randSpeed = minSpeed + static_cast<float>(rand()) / (RAND_MAX / speedRange);

    // Make sure the speed is not 0
    while (randSpeed == 0.0f) {
        randSpeed = minSpeed + static_cast<float>(rand()) / (RAND_MAX / speedRange);
    }

    // Set the entity's transform
    e->cTransform = std::make_shared<CTransform>(randPos, Vec2(randSpeed, randSpeed), 0.0f);

    // Set the entity's score
    e->cScore = std::make_shared<CScore>(100);

    // Generate random number of vertices
    int vertices = m_enemyConfig.VMIN + (rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1));

    // Generate random colors for fill and outline
    sf::Color fillColor = sf::Color(
        static_cast<sf::Uint8>(rand() % 256),
        static_cast<sf::Uint8>(rand() % 256),
        static_cast<sf::Uint8>(rand() % 256)
    );
    sf::Color outlineColor(
        static_cast<sf::Uint8>(m_enemyConfig.OR),
        static_cast<sf::Uint8>(m_enemyConfig.OG),
        static_cast<sf::Uint8>(m_enemyConfig.OB)
    );

    // Set outline thickness
    int outlineThickness = m_enemyConfig.OT;

    // Set the entity's shape
    e->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices, fillColor, outlineColor, outlineThickness);

    // Set the entity's collision
    e->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

    // Record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

//spawn small enemies after shooting it
void Game::spawnSmallEnemies(std::shared_ptr<Entity> parent) {
	// Spawn a number of small enemies equal to the number of vertices of the original
	size_t vertices = parent->cShape->circle.getPointCount();

	Vec2 parentPos(parent->cTransform->pos.x, parent->cTransform->pos.y);
	Vec2 normalizedParentPos = parentPos.normalize();

	sf::Color parentFill = parent->cShape->circle.getFillColor();
	sf::Color parentOutline = parent->cShape->circle.getOutlineColor();
	float parentThickness = parent->cShape->circle.getOutlineThickness();
	float smallEnemyRadius = parent->cShape->circle.getRadius() * 0.5f;
	float smallEnemyCollisionRadius = parent->cCollision->radius * 0.5f;
	float angle = 0;

	for (size_t i = 0; i < vertices; ++i) {
		auto e = m_entities.addEntity("smallEnemy");

		// Small enemies are worth double points of the original enemy
		e->cScore = std::make_shared<CScore>(parent->cScore->score * 2);

		e->cShape = std::make_shared<CShape>(smallEnemyRadius, vertices, parentFill, parentOutline, parentThickness);

		e->cCollision = std::make_shared<CCollision>(smallEnemyCollisionRadius);

		e->cLifespan = std::make_shared<CLifeSpan>(m_enemyConfig.L);

		// Calculate the velocity
		double radians = angle * M_PI / 180.0;
		Vec2 velocity = {
			static_cast<float>(std::cos(radians) * normalizedParentPos.x + std::sin(radians) * normalizedParentPos.y),
			static_cast<float>(std::sin(radians) * normalizedParentPos.x - std::cos(radians) * normalizedParentPos.y)
		};


		// Get the length of the vector
		float L = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);

		// Normalize the vector to get a unit vector
		Vec2 normalizedVelocity = { velocity.x / L, velocity.y / L };

		// Scales the normalized vector by the parent's velocity
		Vec2 newVelocity = { normalizedVelocity.x * parent->cTransform->velocity.x, normalizedVelocity.y * parent->cTransform->velocity.y };

		e->cTransform = std::make_shared<CTransform>(parentPos, newVelocity, 0);

		angle += static_cast<float>(360) / vertices;
	}
}


//spawn the bullet using config setup

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target) {
	auto b = m_entities.addEntity("bullet");

	b->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
	b->cLifespan = std::make_shared<CLifeSpan>(m_bulletConfig.L);
	b->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
		m_bulletConfig.OT);

	Vec2 difference = { target.x - entity->cTransform->pos.x, target.y - entity->cTransform->pos.y };

	Vec2 velocity = { m_bulletConfig.S * difference.normalize().x,m_bulletConfig.S * difference.normalize().y };


	b->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0);
}


void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	float angle = 0;

	for (int j =  0 ; j < m_bulletConfig.SB; ++j)
	{
		auto e = m_entities.addEntity("bullet");

		e->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
			sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
			sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

		e->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

		e->cLifespan = std::make_shared<CLifeSpan>(m_bulletConfig.L);


		Vec2 normalizedPos = m_player->cTransform->pos.normalize();

		//Calculate the velocity

		float radians = static_cast<float>(angle * M_PI / 180);

		Vec2 velocity
		{
			static_cast<float>(std::cos(radians) * normalizedPos.x + std::sin(radians) * normalizedPos.y),
			static_cast<float>(std::sin(radians) * normalizedPos.x - std::cos(radians) * normalizedPos.y)
		};


		//Get the length of the vector 
		float L = velocity.length();

		//Normalize the vector to get an unit vector
		Vec2 normalizedVelocity = { velocity.x / L,velocity.y / L };

		//Scales the normalized vector by the parents velocity
		Vec2 newVelocity = { normalizedVelocity.x * m_bulletConfig.S, normalizedVelocity.y * m_bulletConfig.S };

		e->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos, newVelocity, 0);

		angle += static_cast<float>(360) / m_bulletConfig.SB;
	}
}
