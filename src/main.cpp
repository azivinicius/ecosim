#define CROW_MAIN
#define CROW_STATIC_DIR "../public"

#include "crow_all.h"
#include "json.hpp"
#include <random>
#include <list>

#include "../samples/simulate_random_actions.cpp"

static const uint32_t NUM_ROWS = 15;

// Constants
const uint32_t PLANT_MAXIMUM_AGE = 10;
const uint32_t HERBIVORE_MAXIMUM_AGE = 50;
const uint32_t CARNIVORE_MAXIMUM_AGE = 80;
const uint32_t MAXIMUM_ENERGY = 200;
const uint32_t THRESHOLD_ENERGY_FOR_REPRODUCTION = 20;

// Probabilities
const double PLANT_REPRODUCTION_PROBABILITY = 0.2;
const double HERBIVORE_REPRODUCTION_PROBABILITY = 0.075;
const double CARNIVORE_REPRODUCTION_PROBABILITY = 0.025;
const double HERBIVORE_MOVE_PROBABILITY = 0.7;
const double HERBIVORE_EAT_PROBABILITY = 0.9;
const double CARNIVORE_MOVE_PROBABILITY = 0.5;
const double CARNIVORE_EAT_PROBABILITY = 1.0;

// Type definitions
enum entity_type_t
{
    empty,
    plant,
    herbivore,
    carnivore
};

struct pos_t
{
    uint32_t i;
    uint32_t j;
};

struct entity_t
{
    entity_type_t type;
    int32_t energy;
    int32_t age;
};

// Auxiliary code to convert the entity_type_t enum to a string
NLOHMANN_JSON_SERIALIZE_ENUM(entity_type_t, {
                                                {empty, " "},
                                                {plant, "P"},
                                                {herbivore, "H"},
                                                {carnivore, "C"},
                                            })

// Auxiliary code to convert the entity_t struct to a JSON object
namespace nlohmann
{
    void to_json(nlohmann::json &j, const entity_t &e)
    {
        j = nlohmann::json{{"type", e.type}, {"energy", e.energy}, {"age", e.age}};
    }
}

// Grid that contains the entities
static std::vector<std::vector<entity_t>> entity_grid;

bool reproduct(std::pair<int,int> loc, entity_type_t aux_type, double REPRODUCTION_PROB){
        
        if(random_action(REPRODUCTION_PROB)){
        int a = rand() % 3; //gera 0, 1 ou 2
        if(a!=1){
            if(loc.first+a-1 < NUM_ROWS){
            if(entity_grid[loc.first+a-1][loc.second].type == empty){
                entity_grid[loc.first+a-1][loc.second].type = aux_type;
                entity_grid[loc.first+a-1][loc.second].age = 0;
                entity_grid[loc.first+a-1][loc.second].energy = 0;
                return 1;
            }
            }}
        if(a==1){
            int b = rand() % 3;
            if(b == 1) b=b+1;
            if(loc.second+b-1 < NUM_ROWS){
            if(entity_grid[loc.first][loc.second+b-1].type == empty){
                entity_grid[loc.first][loc.second+b-1].type = aux_type;
                entity_grid[loc.first][loc.second+b-1].age = 0;
                entity_grid[loc.first][loc.second+b-1].energy = 0;
                return 1;
        }
        }}
        return 0;
        }
    }
void kill(std::pair<int,int> loc){
            entity_grid[loc.first][loc.second].type = empty;
            entity_grid[loc.first][loc.second].age = 0;
            entity_grid[loc.first][loc.second].energy = 0;
}
void move(std::pair<int,int> loc, entity_type_t aux_type, double MOVE_PROB){
        if(random_action(MOVE_PROB)){
        
        int a = rand() % 3; //gera 0, 1 ou 2
        if(a!=1){
            if(loc.first+a-1 < NUM_ROWS && loc.first+a-1 >=0){
            if(entity_grid[loc.first+a-1][loc.second].type == empty){
                entity_grid[loc.first+a-1][loc.second].type = aux_type;
                entity_grid[loc.first+a-1][loc.second].age = entity_grid[loc.first][loc.second].age;
                entity_grid[loc.first+a-1][loc.second].energy = entity_grid[loc.first][loc.second].energy-5;
            }
            }}
        else{
            int b = rand() % 3;
            if(b == 1) b=b+1;
            if(loc.second+b-1 < NUM_ROWS && loc.first+-1 >=0){
            if(entity_grid[loc.first][loc.second+b-1].type == empty){
                entity_grid[loc.first][loc.second+b-1].type = aux_type;
                entity_grid[loc.first][loc.second+b-1].age = entity_grid[loc.first][loc.second].age;
                entity_grid[loc.first][loc.second+b-1].energy = entity_grid[loc.first][loc.second].energy-5;
            }
        }
    }
                entity_grid[loc.first][loc.second].type = empty;
                entity_grid[loc.first][loc.second].age = 0;
                entity_grid[loc.first][loc.second].energy = 0;
        }}

void eat(std::pair<int,int> loc, entity_type_t aux_type,
    double EAT_PROB, entity_type_t eaten){
        if(random_action(EAT_PROB)){
        bool ate = 0;
        int a = rand() % 3; //gera 0, 1 ou 2
        if(a!=1){
            if(loc.first+a-1 < NUM_ROWS){
            if(entity_grid[loc.first+a-1][loc.second].type == eaten){
                entity_grid[loc.first+a-1][loc.second].type = aux_type;
                entity_grid[loc.first+a-1][loc.second].age = entity_grid[loc.first][loc.second].age;
                if(aux_type == herbivore)
                entity_grid[loc.first+a-1][loc.second].energy = entity_grid[loc.first][loc.second].energy + 30;
                if(aux_type == carnivore)
                entity_grid[loc.first+a-1][loc.second].energy = entity_grid[loc.first][loc.second].energy + 20;
                ate = true;
            }
            }}
        if(a==1){
            int b = rand() % 3;
            if(b == 1) b=b+1;
            if(loc.second+b-1 < NUM_ROWS){
            if(entity_grid[loc.first][loc.second+b-1].type == eaten){
                entity_grid[loc.first][loc.second+b-1].type = aux_type;
                entity_grid[loc.first][loc.second+b-1].age = entity_grid[loc.first][loc.second].age;
                if(aux_type == herbivore)
                entity_grid[loc.first][loc.second+b-1].energy = entity_grid[loc.first][loc.second].energy + 30;
                if(aux_type == carnivore)
                entity_grid[loc.first][loc.second+b-1].energy = entity_grid[loc.first][loc.second].energy + 20;
                ate = true;
        }
    }}
    if(entity_grid[loc.first][loc.second].energy > MAXIMUM_ENERGY) entity_grid[loc.first][loc.second].energy = MAXIMUM_ENERGY;
    if( ate == true){
            entity_grid[loc.first][loc.second].type = empty;
            entity_grid[loc.first][loc.second].age = 0;
            entity_grid[loc.first][loc.second].energy = 0;    
    }}}

void splants(){
    std::vector<std::pair<int,int>> active;
    for (int i=0; i < NUM_ROWS; i++){
    for (int j=0; j < NUM_ROWS; j++){
        if(entity_grid[i][j].type == plant) active.push_back({i,j}); // Mapeia todas as posições com o tipo passado como parametro

    }}
    for (auto loc : active) {
        if (entity_grid[loc.first][loc.second].age >= 10) kill (loc); //Mata entidade com idade excedente

        else entity_grid[loc.first][loc.second].age = entity_grid[loc.first][loc.second].age + 1; //Incrementa idade dos que não morreram
        
        bool reproducted = reproduct(loc, plant, PLANT_REPRODUCTION_PROBABILITY);

}}

void sherbivores(){
    std::vector<std::pair<int,int>> active;
    for (int i=0; i < NUM_ROWS; i++){
    for (int j=0; j < NUM_ROWS; j++){
        if(entity_grid[i][j].type == herbivore) active.push_back({i,j}); // Mapeia todas as posições com o tipo passado como parametro

    }}
    
    for (auto loc : active) {
        if(entity_grid[loc.first][loc.second].energy == 0) kill (loc); //Mata animal sem energia
        if (entity_grid[loc.first][loc.second].age >= HERBIVORE_MAXIMUM_AGE) kill (loc); //Mata entidade com idade excedente
        else entity_grid[loc.first][loc.second].age = entity_grid[loc.first][loc.second].age + 1; //Incrementa idade dos que não morreram
        reproduct(loc, herbivore, HERBIVORE_REPRODUCTION_PROBABILITY);
        move (loc, herbivore, HERBIVORE_MOVE_PROBABILITY);
        eat (loc, herbivore, HERBIVORE_EAT_PROBABILITY, plant);
    }
}
void simulate(entity_type_t aux_type, uint32_t MAX_AGE, double REPRODUCTION_PROB,
    double MOVE_PROB, double EAT_PROB, entity_type_t eaten){
    std::vector<std::pair<int,int>> active;
    for (int i=0; i < NUM_ROWS; i++){
    for (int j=0; j < NUM_ROWS; j++){
        if(entity_grid[i][j].type == aux_type) active.push_back({i,j}); // Mapeia todas as posições com o tipo passado como parametro

    }}
    
    for (auto loc : active) {
        if(aux_type!=plant){
            if(entity_grid[loc.first][loc.second].energy == 0) kill (loc);} //Mata animal sem energia
        if (entity_grid[loc.first][loc.second].age >= MAX_AGE) kill (loc); //Mata entidade com idade excedente

        else entity_grid[loc.first][loc.second].age = entity_grid[loc.first][loc.second].age + 1; //Incrementa idade dos que não morreram
        
        if (aux_type == plant) 
        bool reproducted = reproduct(loc, aux_type, REPRODUCTION_PROB);

        if(aux_type != plant){
            move(loc, aux_type, MOVE_PROB);
            eat (loc, aux_type,EAT_PROB, eaten);
        } 

        }
        }
    
void run (){
        std::thread s_plant(simulate, plant, PLANT_MAXIMUM_AGE, PLANT_REPRODUCTION_PROBABILITY,
                            0, 0, plant);
        std::thread s_herb(simulate, herbivore, HERBIVORE_EAT_PROBABILITY, HERBIVORE_REPRODUCTION_PROBABILITY,
                            HERBIVORE_MOVE_PROBABILITY, HERBIVORE_EAT_PROBABILITY, plant);
        std::thread s_carn(simulate, carnivore, CARNIVORE_MAXIMUM_AGE, CARNIVORE_REPRODUCTION_PROBABILITY,
                            CARNIVORE_MOVE_PROBABILITY, CARNIVORE_EAT_PROBABILITY, herbivore);    

        s_plant.join();
        s_herb.join();
        s_carn.join();
    }

int main()
{
    crow::SimpleApp app;

    // Endpoint to serve the HTML page
    CROW_ROUTE(app, "/")
    ([](crow::request &, crow::response &res)
     {
        // Return the HTML content here
        res.set_static_file_info_unsafe("../public/index.html");
        res.end(); });

    CROW_ROUTE(app, "/start-simulation")
        .methods("POST"_method)([](crow::request &req, crow::response &res)
                                { 
        // Parse the JSON request body
        nlohmann::json request_body = nlohmann::json::parse(req.body);

       // Validate the request body 
        uint32_t total_entinties = (uint32_t)request_body["plants"] + (uint32_t)request_body["herbivores"] + (uint32_t)request_body["carnivores"];
        if (total_entinties > NUM_ROWS * NUM_ROWS) {
        res.code = 400;
        res.body = "Too many entities";
        res.end();
        return;
        }

        // Clear the entity grid
        entity_grid.clear();
        entity_grid.assign(NUM_ROWS, std::vector<entity_t>(NUM_ROWS, { empty, 0, 0}));
        
        // Create the entities
        // <YOUR CODE HERE>
        // Create the entities randomly
        uint32_t plants_count = (uint32_t)request_body["plants"];
        uint32_t herbivores_count = (uint32_t)request_body["herbivores"];
        uint32_t carnivores_count = (uint32_t)request_body["carnivores"];
        std::map< std::pair<int,int>,bool> grid;
        for (int i=0; i < NUM_ROWS; i++){
            for (int j=0; j < NUM_ROWS; j++){
                grid[{i,j}] = 0;
            }
        }
        // Randomly place plants
        for (uint32_t i = 0; i < plants_count; ++i) {
        bool allocated = 0;
        while (allocated == 0){
            uint32_t x, y;
            x = std::rand() % NUM_ROWS;
            y = std::rand() % NUM_ROWS;
            if(grid[{x,y}]==0){
                entity_grid[x][y] = { plant, 0, 0};
                grid[{x,y}] = 1;
                allocated = 1;
        }}
        }
        // Randomly place herbivores
        for (uint32_t i = 0; i < herbivores_count; ++i) {
            bool allocated = 0;
            while (allocated == 0){
                uint32_t x, y;
                x = std::rand() % NUM_ROWS;
                y = std::rand() % NUM_ROWS;
                if(grid[{x,y}]==0){
                    entity_grid[x][y] = { herbivore, 100, 0 };
                    grid[{x,y}] = 1;
                    allocated = 1;
            }}
        }

        // Randomly place carnivores
        for (uint32_t i = 0; i < carnivores_count; ++i) {
            bool allocated = 0;
            while (allocated == 0){
                uint32_t x, y;
                x = std::rand() % NUM_ROWS;
                y = std::rand() % NUM_ROWS;
                if(grid[{x,y}]==0){
                    entity_grid[x][y] = { carnivore, 100, 0 };
                    grid[{x,y}] = 1;
                    allocated = 1;  
        }}      
        }



        // Return the JSON representation of the entity grid
        nlohmann::json json_grid = entity_grid; 
        res.body = json_grid.dump();
        res.end(); });



    // Endpoint to process HTTP GET requests for the next simulation iteration
    CROW_ROUTE(app, "/next-iteration")
        .methods("GET"_method)([]()
                               {
        // Simulate the next iteration
        // Iterate over the entity grid and simulate the behaviour of each entity
        
        // <YOUR CODE HERE>
  //      s.join();
        simulate (plant, PLANT_MAXIMUM_AGE, PLANT_REPRODUCTION_PROBABILITY,
                            0, 0, plant);
        simulate (herbivore, HERBIVORE_EAT_PROBABILITY, HERBIVORE_REPRODUCTION_PROBABILITY,
                            HERBIVORE_MOVE_PROBABILITY, HERBIVORE_EAT_PROBABILITY, plant);
        simulate (carnivore, CARNIVORE_MAXIMUM_AGE, CARNIVORE_REPRODUCTION_PROBABILITY,
                            CARNIVORE_MOVE_PROBABILITY, CARNIVORE_EAT_PROBABILITY, herbivore);    
        // Return the JSON representation of the entity grid
        nlohmann::json json_grid = entity_grid; 
        return json_grid.dump(); });
    app.port(8080).run();

    return 0;
}


