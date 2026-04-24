#include "groupmanager.h"

GroupManager::~GroupManager() {
    while (head) {
        auto* g = head; head = head->next;
        while (g->members) { auto* m=g->members; g->members=m->next; delete m; }
        delete g;
    }
}

QString GroupManager::generateId() {
    QString id = "SG" + QString::number(nextSeq++).rightJustified(3, '0');
    return id;
}

QString GroupManager::createGroup(QString name, QString courseName, QString courseCode,
    QString desc, QString leaderId, int maxM, QString privacy, QString date) {
    QString gid = generateId();
    auto* g = new GroupNode(gid, name, courseName, courseCode,
                            desc, leaderId, maxM, privacy, date);
    auto* m = new MemberNode(leaderId, "Leader");
    g->members = m; g->currentMembers = 1;
    if (!head) { head = g; return gid; }
    auto* t = head; while (t->next) t = t->next; t->next = g;
    return gid;
}

GroupNode* GroupManager::findById(const QString& id) {
    for (auto* g=head; g; g=g->next) if (g->groupId==id) return g;
    return nullptr;
}

bool GroupManager::deleteGroup(const QString& id) {
    if (!head) return false;
    if (head->groupId == id) {
        auto* t=head; head=head->next;
        while(t->members){auto* m=t->members;t->members=m->next;delete m;}
        delete t; return true;
    }
    for (auto* c=head; c->next; c=c->next)
        if (c->next->groupId == id) {
            auto* t=c->next; c->next=t->next;
            while(t->members){auto* m=t->members;t->members=m->next;delete m;}
            delete t; return true;
        }
    return false;
}

bool GroupManager::closeGroup(const QString& id)  { auto* g=findById(id); if(!g) return false; g->status="Closed";  return true; }
bool GroupManager::reopenGroup(const QString& id) { auto* g=findById(id); if(!g) return false; g->status="Active";  return true; }

bool GroupManager::addMember(const QString& groupId, QString studentId, QString role) {
    auto* g = findById(groupId);
    if (!g || g->currentMembers >= g->maxMembers || g->status=="Closed") return false;
    if (isMember(groupId, studentId)) return false;
    auto* m = new MemberNode(studentId, role);
    m->next = g->members; g->members = m; g->currentMembers++;
    return true;
}

bool GroupManager::removeMember(const QString& groupId, QString studentId) {
    auto* g = findById(groupId); if (!g) return false;
    if (g->members && g->members->studentId == studentId) {
        auto* t=g->members; g->members=t->next; delete t; g->currentMembers--; return true;
    }
    for (auto* m=g->members; m && m->next; m=m->next)
        if (m->next->studentId == studentId) {
            auto* t=m->next; m->next=t->next; delete t; g->currentMembers--; return true;
        }
    return false;
}

bool GroupManager::promoteToLeader(const QString& groupId, QString studentId) {
    auto* g = findById(groupId); if (!g) return false;
    for (auto* m=g->members; m; m=m->next) {
        if (m->studentId == g->leaderStudentId) m->role = "Member";
        if (m->studentId == studentId) { m->role="Leader"; g->leaderStudentId=studentId; }
    }
    return true;
}

bool GroupManager::isMember(const QString& groupId, QString studentId) {
    auto* g = findById(groupId); if (!g) return false;
    for (auto* m=g->members; m; m=m->next) if (m->studentId==studentId) return true;
    return false;
}

QStringList GroupManager::getMembersFormatted(const QString& groupId,
                                               const QMap<QString,QString>& nameMap) {
    QStringList list;
    auto* g = findById(groupId); if (!g) return list;
    for (auto* m=g->members; m; m=m->next) {
        QString name = nameMap.contains(m->studentId) ? nameMap[m->studentId] : "Unknown";
        list << ("  " + name + " | " + m->studentId + " | " + m->role);
    }
    return list;
}

QStringList GroupManager::getAllMembersDetailed(const QString& groupId,
                                                const QMap<QString,QString>& nameMap) {
    return getMembersFormatted(groupId, nameMap);
}

void GroupManager::incrementApprovedEvents(const QString& groupId) {
    auto* g = findById(groupId); if (g) g->approvedEventCount++;
}

bool GroupManager::canRequestMoreEvents(const QString& groupId) {
    auto* g = findById(groupId);
    return g && g->approvedEventCount < 3;
}

QStringList GroupManager::getAllFormatted() {
    QStringList list;
    for (auto* g=head; g; g=g->next)
        list << ("[" + g->groupId + "] " + g->name + " | " + g->courseName +
                " (" + g->courseCode + ") | Members:" +
                QString::number(g->currentMembers) + "/" + QString::number(g->maxMembers) +
                " | " + g->privacy + " | " + g->status +
                " | Events:" + QString::number(g->approvedEventCount) + "/3");
    return list;
}

QStringList GroupManager::getAllDetailed(const QMap<QString,QString>& nameMap) {
    QStringList list;
    for (auto* g=head; g; g=g->next) {
        list << ("=== [" + g->groupId + "] " + g->name + " ===");
        list << ("  Course: " + g->courseName + " (" + g->courseCode +
                ") | Leader: " + g->leaderStudentId +
                " | Members: " + QString::number(g->currentMembers) +
                "/" + QString::number(g->maxMembers));
        list << ("  Privacy: " + g->privacy + " | Status: " + g->status +
                " | Events: " + QString::number(g->approvedEventCount) +
                "/3 | Created: " + g->creationDate);
        list << "  Members:";
        for (auto* m=g->members; m; m=m->next) {
            QString name = nameMap.contains(m->studentId) ? nameMap[m->studentId] : "Unknown";
            list << ("    * " + name + " | " + m->studentId + " | " + m->role);
        }
    }
    return list;
}

QStringList GroupManager::searchByName(const QString& kw) {
    QStringList list;
    for (auto* g=head; g; g=g->next)
        if (g->name.contains(kw,Qt::CaseInsensitive))
            list << ("[" + g->groupId + "] " + g->name + " | " + g->courseName + " | " + g->status);
    return list;
}

QStringList GroupManager::searchByCourse(const QString& course) {
    QStringList list;
    for (auto* g=head; g; g=g->next)
        if (g->courseName.contains(course,Qt::CaseInsensitive)||
            g->courseCode.contains(course,Qt::CaseInsensitive))
            list << ("[" + g->groupId + "] " + g->name + " | " + g->courseName + " " + g->courseCode);
    return list;
}

QStringList GroupManager::getPublicGroups() {
    QStringList list;
    for (auto* g=head; g; g=g->next)
        if (g->privacy=="Public" && g->status=="Active")
            list << ("[" + g->groupId + "] " + g->name + " | " + g->courseName +
                    " | Members:" + QString::number(g->currentMembers) +
                    "/" + QString::number(g->maxMembers) +
                    " | Leader:" + g->leaderStudentId);
    return list;
}

int GroupManager::totalGroups() { int n=0; for(auto* g=head;g;g=g->next) n++; return n; }

GroupNode* GroupManager::getMostActive() {
    GroupNode* best=nullptr;
    for (auto* g=head; g; g=g->next)
        if (!best || g->currentMembers > best->currentMembers) best=g;
    return best;
}

void GroupManager::mergeGroups(const QString& id1, const QString& id2) {
    auto* g1=findById(id1); auto* g2=findById(id2); if (!g1||!g2) return;
    for (auto* m=g2->members; m; m=m->next)
        if (!isMember(id1,m->studentId)) {
            auto* nm=new MemberNode(m->studentId,m->role);
            nm->next=g1->members; g1->members=nm; g1->currentMembers++;
        }
    deleteGroup(id2);
}

static void swapGroupData(GroupNode* a, GroupNode* b) {
    std::swap(a->groupId,b->groupId); std::swap(a->name,b->name);
    std::swap(a->courseName,b->courseName); std::swap(a->courseCode,b->courseCode);
    std::swap(a->description,b->description); std::swap(a->leaderStudentId,b->leaderStudentId);
    std::swap(a->maxMembers,b->maxMembers); std::swap(a->currentMembers,b->currentMembers);
    std::swap(a->privacy,b->privacy); std::swap(a->creationDate,b->creationDate);
    std::swap(a->status,b->status); std::swap(a->approvedEventCount,b->approvedEventCount);
    std::swap(a->members,b->members);
}

void GroupManager::sortByName() {
    if(!head) return; bool sw;
    do { sw=false; for(auto* g=head;g->next;g=g->next) if(g->name>g->next->name){swapGroupData(g,g->next);sw=true;} } while(sw);
}
void GroupManager::sortByMembers() {
    if(!head) return; bool sw;
    do { sw=false; for(auto* g=head;g->next;g=g->next) if(g->currentMembers<g->next->currentMembers){swapGroupData(g,g->next);sw=true;} } while(sw);
}

void GroupManager::appendRaw(GroupNode* node) {
    // Skip duplicates
    if (findById(node->groupId)) { delete node; return; }
    if (!head) head=node; else { auto* t=head; while(t->next) t=t->next; t->next=node; }
    // Update sequence counter from SG-format IDs
    if (node->groupId.startsWith("SG")) {
        int seq = node->groupId.mid(2).toInt();
        if (seq >= nextSeq) nextSeq = seq + 1;
    }
}

void GroupManager::appendMemberRaw(const QString& groupId, MemberNode* m) {
    auto* g=findById(groupId); if (!g) { delete m; return; }
    // Skip duplicate members
    if (isMember(groupId, m->studentId)) { delete m; return; }
    m->next=g->members; g->members=m;
}
