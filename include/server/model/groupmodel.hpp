#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <vector>
#include <string>
using namespace std;

class GroupModel
{
public:
    //创建群组
    bool createGroup(Group &group);
    //加入群组
    void addGroup(int userid, int groupid, string role);
    //查询用户所在群组信息
    vector<Group> queryGroups(int userid);
    //根据groupid查询组内除自己外其他成员id
    vector<int> queryGroupUsers(int userid, int groupid);
};


#endif