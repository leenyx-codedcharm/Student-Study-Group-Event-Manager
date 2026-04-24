#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QString>
#include <QStringList>

// ── Event Node ────────────────────────────────────────────────────────────
struct EventNode {
    int     id;
    QString groupId;
    QString title;
    QString date;
    QString time;
    QString venue;
    QString description;
    QString priority;    // "High" | "Medium" | "Low"
    QString status;      // "Pending" | "Approved" | "Completed" | "Cancelled"
    bool    reminderEnabled;
    QString requestedBy;  // studentId who requested
    QStringList joinedStudents; // students who joined after approval
    EventNode* next;

    EventNode(int i, QString gid, QString t, QString d, QString tm,
              QString v, QString desc, QString prio, QString reqBy)
        : id(i), groupId(gid), title(t), date(d), time(tm),
          venue(v), description(desc), priority(prio),
          status("Pending"), reminderEnabled(true),
          requestedBy(reqBy), next(nullptr) {}
};

// ── Circular reminder node ────────────────────────────────────────────────
struct CircularEventNode {
    int     eventId;
    QString title, date;
    CircularEventNode* next;
    CircularEventNode(int id, QString t, QString d)
        : eventId(id), title(t), date(d), next(nullptr) {}
};

// ── EventManager ──────────────────────────────────────────────────────────
class EventManager {
    EventNode* head = nullptr;
    int nextId = 1;
    CircularEventNode* circHead = nullptr;
    CircularEventNode* circTail = nullptr;

public:
    EventManager() = default;
    ~EventManager();
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    // Request (student with CGPA > 3.5, max 3 per group — checked in mainwindow)
    bool       requestEvent(QString groupId, QString title, QString date, QString time,
                            QString venue, QString desc, QString priority,
                            QString requestedBy);

    EventNode* findById(int id);

    // Admin only
    bool       approveEvent(int id);   // sets status = "Approved"
    bool       deleteEvent(int id);    // admin only after approval

    // Anyone can join an approved event
    bool       joinEvent(int id, QString studentId);
    bool       hasJoined(int id, QString studentId);

    bool       markCompleted(int id);
    bool       cancelEvent(int id);

    // Queries
    QStringList getPendingEvents();                    // admin review queue
    QStringList getApprovedByGroup(const QString& groupId);
    QStringList getUpcoming();
    QStringList getAll();
    QStringList getJoinedStudents(int id);
    int         approvedCountForGroup(const QString& groupId);
    int         totalEvents();
    int         upcomingCount();

    // Circular reminder
    void        buildReminderList();
    QStringList getReminderList();

    // Sorting
    void        sortByDate();
    void        sortByPriority();

    // File helpers
    EventNode* getHead() const { return head; }
    void       appendRaw(EventNode* node);
    void       setNextId(int id) { nextId = id; }
    int        getNextId() const { return nextId; }
};

#endif // EVENTMANAGER_H
