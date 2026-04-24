#ifndef GROUPMANAGER_H
#define GROUPMANAGER_H

#include <QString>
#include <QStringList>
#include <QMap>

struct MemberNode {
    QString studentId;
    QString role;   // "Leader" | "Member"
    MemberNode* next;
    MemberNode(QString id, QString r = "Member")
        : studentId(id), role(r), next(nullptr) {}
};

struct GroupNode {
    QString groupId;     // e.g. "SG001"
    QString name;
    QString courseName;
    QString courseCode;
    QString description;
    QString leaderStudentId;
    int     maxMembers;
    int     currentMembers;
    QString privacy;
    QString creationDate;
    QString status;              // "Active" | "Closed"
    int     approvedEventCount;  // max 3 per group
    MemberNode* members;
    GroupNode*  next;

    GroupNode(QString gid, QString n, QString cn, QString cc,
              QString desc, QString lid, int maxM,
              QString priv, QString date)
        : groupId(gid), name(n), courseName(cn), courseCode(cc),
          description(desc), leaderStudentId(lid), maxMembers(maxM),
          currentMembers(0), privacy(priv), creationDate(date),
          status("Active"), approvedEventCount(0),
          members(nullptr), next(nullptr) {}
};

class GroupManager {
    GroupNode* head = nullptr;
    int nextSeq = 1;  // for SG001, SG002, ...

    QString generateId();  // returns next "SGxxx"

public:
    GroupManager() = default;
    ~GroupManager();
    GroupManager(const GroupManager&) = delete;
    GroupManager& operator=(const GroupManager&) = delete;

    // Returns generated groupId on success, empty string on failure
    QString    createGroup(QString name, QString courseName, QString courseCode,
                           QString desc, QString leaderId, int maxM,
                           QString privacy, QString date);

    GroupNode* findById(const QString& id);
    bool       deleteGroup(const QString& id);
    bool       closeGroup(const QString& id);
    bool       reopenGroup(const QString& id);

    bool        addMember(const QString& groupId, QString studentId, QString role = "Member");
    bool        removeMember(const QString& groupId, QString studentId);
    bool        promoteToLeader(const QString& groupId, QString studentId);
    bool        isMember(const QString& groupId, QString studentId);
    QStringList getMembersFormatted(const QString& groupId,
                                       const QMap<QString,QString>& nameMap = QMap<QString,QString>());
    QStringList getAllMembersDetailed(const QString& groupId,
                                          const QMap<QString,QString>& nameMap = QMap<QString,QString>());

    void        incrementApprovedEvents(const QString& groupId);
    bool        canRequestMoreEvents(const QString& groupId);

    QStringList getAllFormatted();
    QStringList getAllDetailed(const QMap<QString,QString>& nameMap = QMap<QString,QString>());
    QStringList searchByName(const QString& kw);
    QStringList searchByCourse(const QString& course);
    QStringList getPublicGroups();
    int         totalGroups();
    GroupNode*  getMostActive();

    void        mergeGroups(const QString& id1, const QString& id2);
    void        sortByName();
    void        sortByMembers();

    GroupNode* getHead() const { return head; }
    void       appendRaw(GroupNode* node);
    void       appendMemberRaw(const QString& groupId, MemberNode* m);
    void       setNextSeq(int s) { nextSeq = s; }
    int        getNextSeq() const { return nextSeq; }
};

#endif // GROUPMANAGER_H
