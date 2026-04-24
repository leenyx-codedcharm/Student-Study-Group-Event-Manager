#ifndef STUDENTMANAGER_H
#define STUDENTMANAGER_H

#include <QString>
#include <QStringList>

// ─────────────────────────────────────────────
//  Student Node
// ─────────────────────────────────────────────
struct StudentNode {
    QString studentId;   // custom e.g. "2421535042"
    QString fullName;
    QString username;    // firstname + first3digits of ID
    QString password;    // firstname + "123"  (hashed in future)
    QString department;
    QString semester;
    QString email;
    QString phone;
    double  cgpa;
    QString role;        // "Admin" | "Student"
    int     joinedGroups;
    QString lastLogin;
    QString status;      // "Active" | "Blocked"
    StudentNode* next;

    StudentNode(QString sid, QString fn, QString un, QString pw,
                QString dept, QString sem, QString mail,
                QString ph, double cg, QString rl,
                QString ll, QString st)
        : studentId(sid), fullName(fn), username(un), password(pw),
          department(dept), semester(sem), email(mail),
          phone(ph), cgpa(cg), role(rl), joinedGroups(0),
          lastLogin(ll), status(st), next(nullptr) {}
};

// ─────────────────────────────────────────────
//  StudentManager
// ─────────────────────────────────────────────
class StudentManager {
    StudentNode* head = nullptr;

public:
    StudentManager() = default;
    ~StudentManager();
    StudentManager(const StudentManager&) = delete;
    StudentManager& operator=(const StudentManager&) = delete;

    // Registration (admin registers students, generates username/password)
    bool        registerStudent(QString studentId, QString fullName,
                                QString dept, QString sem, QString email,
                                QString phone, double cgpa);
    // Admin self-register or seed
    bool        addAdmin(QString username, QString password, QString fullName);

    // Admin can update any field including username/name/email/id
    bool        adminUpdateStudent(QString studentId, QString fullName,
                                   QString newStudentId, QString username,
                                   QString email, QString dept, QString sem,
                                   QString phone, double cgpa);

    // Student can only update dept, sem, phone, cgpa, password
    bool        studentUpdateProfile(QString studentId, QString dept,
                                     QString sem, QString phone, double cgpa);
    bool        changePassword(QString studentId, QString oldPw, QString newPw);
    bool        adminChangeOwnCredentials(QString adminId, QString newUsername,
                                          QString newPassword, QString currentPw);

    // Find
    StudentNode* findById(const QString& id);
    StudentNode* findByUsername(const QString& username);

    // Delete / block (admin can't be deleted)
    bool        deleteStudent(const QString& id);
    bool        blockStudent(const QString& id);
    bool        unblockStudent(const QString& id);

    // Auth
    StudentNode* login(const QString& username, const QString& password);
    void         updateLastLogin(const QString& id, const QString& timestamp);

    // Group count helpers
    void        incrementGroupCount(const QString& id);
    void        decrementGroupCount(const QString& id);

    // Queries
    QStringList getAllFormatted();
    QStringList searchByName(const QString& kw);
    QStringList searchByDept(const QString& dept);
    int         totalStudents();   // excludes admins
    StudentNode* getMostActive();
    bool        usernameExists(const QString& un);
    bool        studentIdExists(const QString& id);

    // Advanced list ops
    void        reverseList();
    void        removeDuplicates();

    // Sorting
    void        sortByName();
    void        sortById();
    void        sortByCGPA();

    // File helpers
    StudentNode* getHead() const { return head; }
    void         appendRaw(StudentNode* node);

    // Username/password generation helpers (public so UI can show them)
    static QString generateUsername(const QString& fullName, const QString& studentId);
    static QString generatePassword(const QString& fullName);
};

#endif // STUDENTMANAGER_H
