/**
 * SPACESIM!
 * N-body physics solver
 *
 * This module implements the N-body physics solver for simulating gravitational interactions between bodies in space.
 *
 * @author The glitched engineers
 */

#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "solver.h"

#define G 6.67430e-11f // Gravitational constant (in m^3 kg^-1 s^-2)

OrbitalSim *CreateOrbitalSim(uint32_t num_bodies, double initial_time)
{
    OrbitalSim *sim = new OrbitalSim;

    sim->num_bodies = num_bodies;
    sim->bodies = new Body[num_bodies];
    
    sim->time = initial_time;

    return sim;
}

/**
 * @brief Extracts the ephemeris time from a CSV filename
 *
 * Parses the filename (last path component) in the format YYYY-MM-DD_HH-MM-SS.csv
 * to extract the simulation epoch time.
 *
 * @param filepath Path to the file (may contain relative or absolute path components)
 * @return Unix epoch timestamp in seconds for the parsed time, or 0.0 if parsing fails
 */
double ExtractEphemerisTimeFromFilename(const char *filepath)
{
    // Extract only the last path component (filename)
    std::string filepath_str(filepath);
    size_t last_sep = filepath_str.find_last_of("/\\");
    std::string filename = (last_sep != std::string::npos)
        ? filepath_str.substr(last_sep + 1)
        : filepath_str;

    // Parse the time from the filename (expected format: YYYY-MM-DD_HH-MM-SS.csv)
    std::tm tm_info = {};
    std::istringstream ss(filename);
    ss >> std::get_time(&tm_info, "%Y-%m-%d_%H-%M-%S");

    if (!ss.fail())
    {
        time_t time = mktime(&tm_info);
        return static_cast<double>(time);
    }
    else
        return 0.0; // default to epoch if parsing fails
}

OrbitalSim *CreateOrbitalSimFromFile(const char *filename)
{
    // Open CSV file
    std::ifstream file(filename);
    if (!file.is_open())
        return nullptr;

    // Load CSV file
    std::vector<Body> bodies;
    std::string line;
    bool is_header_line = true;

    while (std::getline(file, line))
    {
        // Skip empty lines
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos)
            continue;

        // Skip header line
        if (is_header_line)
        {
            is_header_line = false;
            continue;
        }

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> fields; // C++ vector to hold the split fields

        // Split by comma
        while (std::getline(ss, token, ','))
        {
            // Remove leading/trailing whitespace
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t\r\n") + 1);
            fields.push_back(token);
        }

        // We expect at least 8 fields (name + mass + 3 position + 3 velocity)
        if (fields.size() < 8)
            continue;

        Body body;
        try
        {
            // Name
            // fields[0] ignored for simulation

            // Mass
            body.mass = stod(fields[1]);

            // Mean radius (convert from km to m)
            body.mean_radius = 1000 * stod(fields[2]);

            // Position (convert from km to m)
            body.position[0] = 1000 * stod(fields[3]);
            body.position[1] = 1000 * stod(fields[4]);
            body.position[2] = 1000 * stod(fields[5]);

            // Velocity (convert from km/s to m/s)
            body.velocity[0] = 1000 * stod(fields[6]);
            body.velocity[1] = 1000 * stod(fields[7]);
            body.velocity[2] = 1000 * stod(fields[8]);

            bodies.push_back(body);
        }
        catch (const std::exception &e)
        {
            continue;
        }
    }

    file.close();

    // Extract initial time from file path
    double initial_time = ExtractEphemerisTimeFromFilename(filename);

    // Create and populate simulation structure
    OrbitalSim *sim = CreateOrbitalSim(bodies.size(), initial_time);

    for (uint32_t i = 0; i < bodies.size(); ++i)
        sim->bodies[i] = bodies[i];

    return sim;
}

void DestroyOrbitalSim(OrbitalSim *sim)
{
    delete[] sim->bodies;

    delete sim;
}

void SimulateOrbitalSimStep(OrbitalSim *sim, float time_step)
{
    uint32_t N = sim->num_bodies;
    Body *bodies = sim->bodies;

    for (uint32_t i = 0; i < N; i++)
    {
        break;

        Body *body = &sim->bodies[i];

        float acceleration[3] = {0.0f, 0.0f, 0.0f};

        for (uint32_t j = 0; j < N; j++)
        {
            for (uint32_t k = 0; k < N; k++)
            {
                if (j == k)
                    continue;

                float dx = bodies[j].position[0] - bodies[k].position[0];
                float dy = bodies[j].position[1] - bodies[k].position[1];
                float dz = bodies[j].position[2] - bodies[k].position[2];

                float r2 = dx * dx + dy * dy + dz * dz;
                float force = G * (bodies[j].mass * bodies[k].mass) / r2;

                float r = sqrtf(r2);
                float rx = dx / r;
                float ry = dy / r;
                float rz = dz / r;

                acceleration[0] += force * rx / bodies[j].mass;
                acceleration[1] += force * ry / bodies[j].mass;
                acceleration[2] += force * rz / bodies[j].mass;

                body->position[0] += body->velocity[0] * time_step;
                body->position[1] += body->velocity[1] * time_step;
                body->position[2] += body->velocity[2] * time_step;

                body->velocity[0] += acceleration[0] * time_step;
                body->velocity[1] += acceleration[1] * time_step;
                body->velocity[2] += acceleration[2] * time_step;
            }
        }
    }

    sim->time += time_step;
}
