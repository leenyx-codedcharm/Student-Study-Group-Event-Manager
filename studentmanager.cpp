#include "studentmanager.h"
#include <QDateTime>

StudentManager::~StudentManager() {
    while (head) { auto* t = head; head = head->next; delete t; }
}

// ── Username / Password generation ───────────────────────────────────────
QString StudentManager::generateUsername(const QString& fullName, const QString& studentId) {
    // firstname (lowercase) + first 3 digits of studentId
    QString first = fullName.trimmed().split(" ").first().toLower();
    QString digits = studentId.left(3);
    return first + digits;
}

QString StudentManager::generatePassword(const QString& fullName) {
    // firstname (lowercase) + "123"
    QString first = fullName.trimmed().split(" ").first().toLower();
    return first + "123";
}

// ── Register student (admin registers) ───────────────────────────────────
bool StudentManager::registerStudent(QString studentId, QString fullName,
    QString dept, QString sem, QString email, QString phone, double cgpa) {

    if (studentIdExists(studentId)) return false;

    QString un = generateUsername(fullName, studentId);
    QString pw = generatePassword(fullName);

    // Make username unique if collision
    QString baseUn = un;
    int suffix = 1;
    while (usernameExists(un)) un = baseUn + QString::number(suffix++);

    QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    auto* node = new StudentNode(studentId, fullName, un, pw,
                                 dept, sem, email, phone, cgpa,
                                 "Student", ts, "Active");
    if (!head) { head = node; return true; }
    auto* t = head; while (t->next) t = t->next; t->next = node;
    return true;
}

bool StudentManager::addAdmin(QString username, QString password, QString fullName) {
    if (usernameExists(username)) return false;
    QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
    auto* node = new StudentNode("ADMIN", fullName, username, password,
                                 "Admin", "N/A", "admin@system.edu",
                                 "0000000000", 4.0, "Admin", ts, "Active");
    if (!head) { head = node; return true; }
    auto* t = head; while (t->next) t = t->next; t->next = node;
    return true;
}

// ── Find ──────────────────────────────────────────────────────────────────
StudentNode* StudentManager::findById(const QString& id) {
    for (auto* c = head; c; c = c->next) if (c->studentId == id) return c;
    return nullptr;
}
StudentNode* StudentManager::findByUsername(const QString& un) {
    for (auto* c = head; c; c = c->next) if (c->username == un) return c;
    return nullptr;
}

// ── Auth ──────────────────────────────────────────────────────────────────
StudentNode* StudentManager::login(const QString& un, const QString& pw) {
    auto* s = findByUsername(un);
    if (!s || s->password != pw || s->status == "Blocked") return nullptr;
    updateLastLogin(s->studentId,
        QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    return s;
}

void StudentManager::updateLastLogin(const QString& id, const QString& ts) {
    auto* s = findById(id); if (s) s->lastLogin = ts;
}

// ── Updates ───────────────────────────────────────────────────────────────
bool StudentManager::adminUpdateStudent(QString studentId, QString fullName,
    QString newStudentId, QString username, QString email,
    QString dept, QString sem, QString phone, double cgpa) {
    auto* s = findById(studentId); if (!s) return false;
    // Check new ID/username don't conflict with others
    if (newStudentId != studentId && studentIdExists(newStudentId)) return false;
    if (username != s->username && usernameExists(username)) return false;
    s->studentId   = newStudentId;
    s->fullName    = fullName;
    s->username    = username;
    s->email       = email;
    s->department  = dept;
    s->semester    = sem;
    s->phone       = phone;
    s->cgpa        = cgpa;
    return true;
}

bool StudentManager::studentUpdateProfile(QString studentId, QString dept,
    QString sem, QString phone, double cgpa) {
    auto* s = findById(studentId); if (!s) return false;
    s->department = dept; s->semester = sem;
    s->phone = phone; s->cgpa = cgpa;
    return true;
}

bool StudentManager::changePassword(QString studentId, QString oldPw, QString newPw) {
    auto* s = findById(studentId); if (!s) return false;
    if (s->password != oldPw) return false;
    s->password = newPw; return true;
}

bool StudentManager::adminChangeOwnCredentials(QString adminId, QString newUsername,
    QString newPassword, QString currentPw) {
    auto* s = findById(adminId); if (!s || s->role != "Admin") return false;
    if (s->password != currentPw) return false;
    if (newUsername != s->username && usernameExists(newUsername)) return false;
    s->username = newUsername;
    s->password = newPassword;
    return true;
}

bool StudentManager::deleteStudent(const QString& id) {
    auto* s = findById(id);
    if (!s || s->role == "Admin") return false; // admin can't be deleted
    if (head->studentId == id) { auto* t = head; head = head->next; delete t; return true; }
    for (auto* c = head; c->next; c = c->next)
        if (c->next->studentId == id) {
            auto* t = c->next; c->next = t->next; delete t; return true;
        }
    return false;
}

bool StudentManager::blockStudent(const QString& id) {
    auto* s = findById(id);
    if (!s || s->role == "Admin") return false;
    s->status = "Blocked"; return true;
}

bool StudentManager::unblockStudent(const QString& id) {
    auto* s = findById(id); if (!s) return false;
    s->status = "Active"; return true;
}

void StudentManager::incrementGroupCount(const QString& id) {
    auto* s = findById(id); if (s) s->joinedGroups++;
}
void StudentManager::decrementGroupCount(const QString& id) {
    auto* s = findById(id); if (s && s->joinedGroups > 0) s->joinedGroups--;
}

// ── Queries ───────────────────────────────────────────────────────────────
QStringList StudentManager::getAllFormatted() {
    QStringList list;
    for (auto* c = head; c; c = c->next)
        if (c->role != "Admin")
            list << QString("[%1] %2 | %3 | %4 | CGPA:%5 | %6 | %7")
                    .arg(c->studentId).arg(c->fullName).arg(c->username)
                    .arg(c->department).arg(c->cgpa, 0,'f',2)
                    .arg(c->semester).arg(c->status);
    return list;
}

QStringList StudentManager::searchByName(const QString& kw) {
    QStringList list;
    for (auto* c = head; c; c = c->next)
        if (c->role != "Admin" &&
            (c->fullName.contains(kw, Qt::CaseInsensitive) ||
             c->username.contains(kw, Qt::CaseInsensitive) ||
             c->studentId.contains(kw, Qt::CaseInsensitive)))
            list << ("[" + c->studentId + "] " + c->fullName + " | " + c->username +
                    " | " + c->department + " | CGPA:" + QString::number(c->cgpa,'f',2));
    return list;
}

QStringList StudentManager::searchByDept(const QString& dept) {
    QStringList list;
    for (auto* c = head; c; c = c->next)
        if (c->role != "Admin" &&
            c->department.contains(dept, Qt::CaseInsensitive))
            list << QString("[%1] %2 | %3").arg(c->studentId).arg(c->fullName).arg(c->department);
    return list;
}

int StudentManager::totalStudents() {
    int n = 0;
    for (auto* c = head; c; c = c->next) if (c->role != "Admin") n++;
    return n;
}

StudentNode* StudentManager::getMostActive() {
    StudentNode* best = nullptr;
    for (auto* c = head; c; c = c->next)
        if (c->role != "Admin" && (!best || c->joinedGroups > best->joinedGroups))
            best = c;
    return best;
}

bool StudentManager::usernameExists(const QString& un) { return findByUsername(un) != nullptr; }
bool StudentManager::studentIdExists(const QString& id) { return findById(id) != nullptr; }

// ── Advanced ops ──────────────────────────────────────────────────────────
void StudentManager::reverseList() {
    StudentNode *prev=nullptr, *curr=head, *next=nullptr;
    while (curr) { next=curr->next; curr->next=prev; prev=curr; curr=next; }
    head = prev;
}

void StudentManager::removeDuplicates() {
    for (auto* c = head; c; c = c->next)
        for (auto* p = c; p->next;)
            if (p->next->studentId == c->studentId) {
                auto* dup = p->next; p->next = dup->next; delete dup;
            } else p = p->next;
}

// Helper: swap all data fields between two nodes
static void swapNodes(StudentNode* a, StudentNode* b) {
    std::swap(a->studentId, b->studentId);
    std::swap(a->fullName,  b->fullName);
    std::swap(a->username,  b->username);
    std::swap(a->password,  b->password);
    std::swap(a->department,b->department);
    std::swap(a->semester,  b->semester);
    std::swap(a->email,     b->email);
    std::swap(a->phone,     b->phone);
    std::swap(a->cgpa,      b->cgpa);
    std::swap(a->role,      b->role);
    std::swap(a->joinedGroups, b->joinedGroups);
    std::swap(a->lastLogin, b->lastLogin);
    std::swap(a->status,    b->status);
}

void StudentManager::sortByName() {
    if (!head) return;
    bool sw; do { sw=false;
        for (auto* c=head; c->next; c=c->next)
            if (c->fullName > c->next->fullName) { swapNodes(c,c->next); sw=true; }
    } while(sw);
}
void StudentManager::sortById() {
    if (!head) return;
    bool sw; do { sw=false;
        for (auto* c=head; c->next; c=c->next)
            if (c->studentId > c->next->studentId) { swapNodes(c,c->next); sw=true; }
    } while(sw);
}
void StudentManager::sortByCGPA() {
    if (!head) return;
    bool sw; do { sw=false;
        for (auto* c=head; c->next; c=c->next)
            if (c->cgpa < c->next->cgpa) { swapNodes(c,c->next); sw=true; }
    } while(sw);
}

void StudentManager::appendRaw(StudentNode* node) {
    // Skip if a node with the same studentId already exists (prevents load duplicates)
    if (studentIdExists(node->studentId)) { delete node; return; }
    if (!head) head = node;
    else { auto* t=head; while(t->next) t=t->next; t->next=node; }
}
