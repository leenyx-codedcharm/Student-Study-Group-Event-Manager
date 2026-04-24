#include "eventmanager.h"

EventManager::~EventManager() {
    while (head) { auto* t=head; head=head->next; delete t; }
    if (circHead) {
        circTail->next = nullptr;
        while (circHead) { auto* t=circHead; circHead=circHead->next; delete t; }
    }
}

bool EventManager::requestEvent(QString groupId, QString title, QString date,
    QString time, QString venue, QString desc, QString priority, QString requestedBy) {
    auto* e = new EventNode(nextId++, groupId, title, date, time,
                            venue, desc, priority, requestedBy);
    if (!head) { head=e; return true; }
    auto* t=head; while(t->next) t=t->next; t->next=e;
    return true;
}

EventNode* EventManager::findById(int id) {
    for (auto* e=head; e; e=e->next) if (e->id==id) return e;
    return nullptr;
}

bool EventManager::approveEvent(int id) {
    auto* e=findById(id); if (!e) return false;
    e->status = "Approved"; return true;
}

bool EventManager::deleteEvent(int id) {
    if (!head) return false;
    if (head->id==id) { auto* t=head; head=head->next; delete t; return true; }
    for (auto* c=head; c->next; c=c->next)
        if (c->next->id==id) { auto* t=c->next; c->next=t->next; delete t; return true; }
    return false;
}

bool EventManager::joinEvent(int id, QString studentId) {
    auto* e=findById(id);
    if (!e || e->status != "Approved") return false;
    if (hasJoined(id, studentId)) return false;
    e->joinedStudents.append(studentId);
    return true;
}

bool EventManager::hasJoined(int id, QString studentId) {
    auto* e=findById(id); if (!e) return false;
    return e->joinedStudents.contains(studentId);
}

bool EventManager::markCompleted(int id) { auto* e=findById(id); if(!e) return false; e->status="Completed"; return true; }
bool EventManager::cancelEvent(int id)   { auto* e=findById(id); if(!e) return false; e->status="Cancelled"; return true; }

QStringList EventManager::getPendingEvents() {
    QStringList list;
    for (auto* e=head; e; e=e->next)
        if (e->status=="Pending")
            list << (QString("[") + QString::number(e->id) + "] " + e->title + " | Group:" + e->groupId + " | " + e->date + " | By:" + e->requestedBy + " | " + e->priority);
    return list;
}

QStringList EventManager::getApprovedByGroup(const QString& groupId) {
    QStringList list;
    for (auto* e=head; e; e=e->next)
        if (e->groupId==groupId && e->status=="Approved")
            list << (QString("[") + QString::number(e->id) + "] " + e->title + " | " + e->date + " " + e->time + " | " + e->venue + " | Joined:" + QString::number(e->joinedStudents.size()));
    return list;
}

QStringList EventManager::getUpcoming() {
    QStringList list;
    for (auto* e=head; e; e=e->next)
        if (e->status=="Approved")
            list << (QString("[") + QString::number(e->id) + "] " + e->title + " | " + e->date + " | Group:" + e->groupId + " | " + e->priority);
    return list;
}

QStringList EventManager::getAll() {
    QStringList list;
    for (auto* e=head; e; e=e->next)
        list << (QString("[") + QString::number(e->id) + "] " + e->title + " | Group:" + e->groupId + " | " + e->date + " | " + e->priority + " | " + e->status);
    return list;
}

QStringList EventManager::getJoinedStudents(int id) {
    auto* e=findById(id); if (!e) return {};
    QStringList list;
    for (const QString& s : e->joinedStudents)
        list << "StudentID: " + s;
    return list;
}

int EventManager::approvedCountForGroup(const QString& groupId) {
    int n=0;
    for (auto* e=head; e; e=e->next)
        if (e->groupId==groupId && e->status=="Approved") n++;
    return n;
}

int EventManager::totalEvents() { int n=0; for(auto* e=head;e;e=e->next) n++; return n; }
int EventManager::upcomingCount() { int n=0; for(auto* e=head;e;e=e->next) if(e->status=="Approved") n++; return n; }

void EventManager::buildReminderList() {
    if (circHead) { circTail->next=nullptr; while(circHead){auto* t=circHead;circHead=circHead->next;delete t;} circHead=circTail=nullptr; }
    for (auto* e=head; e; e=e->next)
        if (e->status=="Approved" && e->reminderEnabled) {
            auto* cn=new CircularEventNode(e->id,e->title,e->date);
            if (!circHead) { circHead=circTail=cn; cn->next=cn; }
            else { circTail->next=cn; cn->next=circHead; circTail=cn; }
        }
}

QStringList EventManager::getReminderList() {
    QStringList list; if (!circHead) return list;
    auto* cur=circHead;
    do { list << QString("🔔 %1 on %2").arg(cur->title).arg(cur->date); cur=cur->next; }
    while (cur!=circHead);
    return list;
}

void EventManager::sortByDate() {
    if (!head) return; bool sw;
    do { sw=false;
        for (auto* e=head; e->next; e=e->next)
            if (e->date > e->next->date) {
                std::swap(e->id,e->next->id); std::swap(e->groupId,e->next->groupId);
                std::swap(e->title,e->next->title); std::swap(e->date,e->next->date);
                std::swap(e->time,e->next->time); std::swap(e->venue,e->next->venue);
                std::swap(e->description,e->next->description);
                std::swap(e->priority,e->next->priority); std::swap(e->status,e->next->status);
                std::swap(e->reminderEnabled,e->next->reminderEnabled);
                std::swap(e->requestedBy,e->next->requestedBy);
                std::swap(e->joinedStudents,e->next->joinedStudents);
                sw=true;
            }
    } while(sw);
}

void EventManager::sortByPriority() {
    auto rank=[](const QString& p){return p=="High"?0:p=="Medium"?1:2;};
    if (!head) return; bool sw;
    do { sw=false;
        for (auto* e=head; e->next; e=e->next)
            if (rank(e->priority) > rank(e->next->priority)) {
                std::swap(e->id,e->next->id); std::swap(e->groupId,e->next->groupId);
                std::swap(e->title,e->next->title); std::swap(e->date,e->next->date);
                std::swap(e->time,e->next->time); std::swap(e->venue,e->next->venue);
                std::swap(e->description,e->next->description);
                std::swap(e->priority,e->next->priority); std::swap(e->status,e->next->status);
                std::swap(e->reminderEnabled,e->next->reminderEnabled);
                std::swap(e->requestedBy,e->next->requestedBy);
                std::swap(e->joinedStudents,e->next->joinedStudents);
                sw=true;
            }
    } while(sw);
}

void EventManager::appendRaw(EventNode* node) {
    if (!head) head=node; else { auto* t=head; while(t->next) t=t->next; t->next=node; }
    if (node->id >= nextId) nextId=node->id+1;
}
