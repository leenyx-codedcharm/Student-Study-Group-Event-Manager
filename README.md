# 📚 Student Study Group & Event Manager

A desktop application built with **Qt6 + C++** for managing student study groups, events, tasks, and members — with all core data structures implemented as custom **singly and circular linked lists** (no STL containers).

Developed as the **CSE225: Data Structures** term project at North South University (NSU), Spring 2026.

---

## ✨ Features

### 👤 Admin
- Register, update, and block/unblock students
- Create and manage study groups
- View and manage events & tasks across all groups
- Change admin credentials

### 🎓 Student
- Login and view personal profile
- Browse, join, and leave study groups
- Create, edit, and delete events within joined groups
- Create, update, and mark tasks as complete
- Search groups by name/course, sort groups alphabetically
- View event reminders via circular linked list traversal

---

## 🏗️ Project Structure

```
StudentStudyGroupManager/
├── main.cpp                  # Entry point
├── mainwindow.cpp / .h / .ui # Main Qt window and all UI logic
├── studentmanager.cpp / .h   # Student linked list operations
├── groupmanager.cpp / .h     # Group linked list + nested member lists
├── eventmanager.cpp / .h     # Event linked list + circular reminder list
├── taskmanager.cpp / .h      # Task linked list operations
├── filehandler.cpp / .h      # File persistence (save/load/backup)
└── CMakeLists.txt            # CMake build configuration
```

---

## 🔧 Prerequisites

| Tool | Version |
|------|---------|
| Qt | 6.x (Widgets module) |
| CMake | 3.16 or higher |
| C++ Compiler | C++17 compatible (MinGW / MSVC / GCC / Clang) |
| Qt Creator | Recommended IDE (any version supporting Qt6) |

---

## 🚀 Build & Run

### Option A — Qt Creator (Recommended)

1. Open **Qt Creator**
2. Click **File → Open File or Project**
3. Select `CMakeLists.txt` from the project folder
4. Click **Configure Project** and choose your Qt6 kit
5. Press **Ctrl+R** (or the green ▶ button) to build and run

### Option B — Command Line

```bash
# From inside the project folder
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt6
cmake --build .
./StudentStudyGroupManager        # Linux/macOS
StudentStudyGroupManager.exe      # Windows
```

> Replace `/path/to/Qt6` with your actual Qt6 installation path,  
> e.g. `C:/Qt/6.7.0/mingw_64` on Windows.

---

## 💾 Data Persistence

All data is automatically saved to plain text files in:

```
~/StudyGroupData/
├── students.txt
├── groups.txt
├── events.txt
└── tasks.txt
    backup/         ← auto-backup snapshots
```

The app saves automatically after every create / update / delete operation. Data is reloaded on next launch.

---

## 🗂️ Data Structure Overview

| Structure | Used For |
|-----------|----------|
| Singly Linked List | Students, Groups, Events, Tasks |
| Nested Linked List | Group → Members, Group → Events, Group → Tasks |
| Circular Linked List | Event reminder traversal |

All insertion, deletion, search, and sort operations are implemented manually using pointer traversal — no `std::list`, `std::vector`, or other STL containers are used for core data.

---

## 🐛 Known Issues & Fixes

- **"Task not found" on Update/Done buttons** — Caused by typing `T1` instead of `1` in the Task ID field. Fixed by stripping the `T` prefix before parsing, and by the `on_listTasks_itemClicked` slot that auto-fills the ID when you click a task in the list.

---

## 📄 License

Copyright © 2026 **Asfee Bhuiyan Leen**

This project was developed for academic purposes as part of the CSE225: Data Structures course at **North South University (NSU)**, Dhaka, Bangladesh.

You are free to use, study, and reference this code for learning purposes. Redistribution or submission of this work as your own academic assignment is not permitted.
