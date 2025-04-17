# Kart-It Project

Welcome to the Kart-It Project! This repository contains all the files and resources needed to understand, navigate, and reproduce the project. Below is an overview of the directory structure and its contents.

---

## Team Members

Meet the individuals who contributed to the Kart-It Project:

- **William Roper**
- **Zachary McPherson**
- **Karthik Raja**

Feel free to contact any of us for project-related questions or collaborations!

## Directory Overview

### CAD Files
This folder contains all CAD designs for the physical components of the system.

#### 3D Models
- **Pedals:** STL files for the gas and brake pedals.
- **Shift Gear:** STL files for the shift gear components.
- **Steering Wheel:** STL files for the steering wheel and steering column.

#### Laser Cut Files
- DXF files for the system's enclosure, designed for laser cutting on the X-Carve machine using plywood.

---

### Hardware Files
This folder contains all PCB-related files for the project, designed using Altium.

- **PCB Altium Files:** Includes schematic files, PCB layout files, project output files, Design Rule Check files, GERBER files, and NC drill files.

---

### Software Files
This folder contains the software programs used in the Delta_BopIt system.

#### Helper Programs
- Helper functions for individual sensors and components used in the project.

#### Main Program
- The main program uploaded to the ATMega328P chip integrated into the PCB.

#### Emulator Program
- The main program uploaded to the Raspberry Pi to run the Mario Kart emulator.

---

## Reproduction Instructions
1. **CAD Setup:** Use the STL and DXF files to fabricate the physical components using a 3D printer or laser cutter.
2. **PCB Fabrication:** Utilize the GERBER files and NC drill files to manufacture the PCB. Ensure proper assembly based on the schematic files.
3. **Software Deployment:** Load the main program onto the ATMega328P chip on the PCB. Test individual components using the helper functions.

Feel free to explore the directories for more specific details. If you encounter any issues or have questions, refer to the README.txt file for additional guidance.

---
