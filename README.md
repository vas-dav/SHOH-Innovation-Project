# SHOH-Innovation-Project
Smart Home Oil Heater is a project for Metropolia UAS course called Multidisciplinary Innovation Project.

Tasks communication:

Rotary: gets rotation dir --- Master --- Temp Task: gets temp
                                |
                                |
                                |
                                |
                                |
Menu Task ------------------------------------------- Heater Task




common struct:

typedef struct {
  int8_t temp_real;
} sensor_temp ;

typedef struct {
  bool right;
  bool press;
} rotary_action ;

typedef struct {
  int8_t set_point;
} user_settings;


common struct communication diagram:

- sensor_temp  (Temp Task --> Master --> Menu && Heater)
- rotary_action (Rotary --> Master --> Menu Task)
- user_settings (Menu --> Master --> Heater)

