# Azulindo

Azulindo is a personal **offline car AI project** focused on building an interactive visual interface using **C++ and Raylib**.  
The project also includes experiments with local AI inference using **llama.cpp**.

The goal is to create an AI that represents the *personality of the car*, visually and behaviorally reacting to passenger interactions and internal system states.

The interface visually reflects the AI state, where motion and behavior communicate how the system is processing information.

---

## Project organization and vision

Azulindo is designed as a fully offline AI system that acts as a digital personality for the car.  
The system reacts to passenger input while also storing mechanical and usage-related information, such as oil change dates and part replacements.

Development follows an incremental and modular approach, divided into the following stages:

### 1. User Interface (UI)
- Build the visual interface using **Raylib**
- Define screen dynamics and visual reactions to speech and system activity
- Establish visual behavior as the foundation of the AI’s emotional and behavioral expression

### 2. Local LLM experiments
- Test different local LLMs after a minimal interactive UI is established
- Focus on embeddability and resource usage, especially **RAM consumption**
- Use **llama.cpp** as the base for local inference experiments

### 3. Speech pipeline
- **Speech-to-Text**: listening and transcribing spoken input
- Feeding the transcribed input into the **LLM** for processing and response generation
- **Text-to-Speech**: verbal system responses

### 4. State & memory
- Store mechanical and usage information
- Maintain short- and long-term contextual data for interactions and behavior

Each stage is developed incrementally, keeping the system modular, testable, and adaptable for future expansion.

---

## Tech stack

- **C++**
- **Raylib** – graphics and interaction
- **llama.cpp** – local LLM inference
- **CMake** – build system

---

## Status

🚧 Early development  
Current focus: visual behavior, modular architecture, and interface dynamics.

---

## Wave system (early tests)

Below are early test stages of the wave system, which represent the AI’s emotional and behavioral states through dynamic visual motion.

![Wave system](docs/interface.gif)
