# 🧮 SmartCalc – Personal Finance Tracker in C (GTK4)

**SmartCalc** is a modern personal finance tracker built in **C using GTK4**. It helps users manage income and expenses through a clean graphical interface — far beyond a basic calculator.

---

## ✨ Features

- 💡 Add income & expense entries with meaningful **labels**
- 🔀 Toggle between **Income** and **Expense**
- 📁 All records saved to a **CSV** file for persistence
- 🖼️ Modern GTK4 **Graphical User Interface**
- 🔄 Real-time balance, totals, and updates
- 📂 Simple flat project structure in `main.c`

---

## 🧪 How to Run

### 🧰 Prerequisites

- Linux system (Ubuntu recommended)
- `gcc` and `gtk4` libraries installed

```bash
sudo apt update
sudo apt install build-essential libgtk-4-dev
```

### ▶️ Compile & Run

```bash
gcc `pkg-config --cflags gtk4` -o smartcalc main.c `pkg-config --libs gtk4`
./smartcalc
```

---

## 💼 Use Cases

- Monthly budget tracking (e.g., salary, groceries, rent)
- Event expense planning (e.g., trips, parties)
- Reviewing past records for better financial awareness

---

## 🧠 Learning Highlights

- GTK4 GUI development in C  
- File handling with CSV  
- Structs, pointers, and memory management  
- Modular thinking within a single-file design

---

## 👨‍💻 Developers

- **Mohammed Tahir Siddique** – [LinkedIn](https://www.linkedin.com/in/mohammed-tahir-siddique/)  
- Hitha Shree Suresh  
- Mann Mehta  
- Mokshith Reddy

---
