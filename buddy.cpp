#include<iostream>
#include<stdlib.h>
#include<vector>
#include<math.h>    //      ceil / log
#include<map>
using namespace std;

int siz=0;//
vector<pair<int, int>> free_list[1000000];
map<int, int> mp;

void initialize(int sizee){
    int n = ceil(log(sizee)/log(2));
    siz = n+1;

    for(int i=0; i <= n; i++)  
        free_list[i].clear();

    free_list[n].push_back(make_pair(0, sizee-1));
    cout << "Size of " << sizee << " initialized.\n";
}

void allocate(int sizee){
    int n = ceil(log(sizee)/log(2));
    if(free_list[n].size() > 0)
    {
        pair<int, int> temp = free_list[n][0];
        free_list[n].erase(free_list[n].begin());
        mp[temp.first] = temp.second - temp.first + 1;
        cout << "Memory from " << temp.first << " to " <<
        temp.second << " allocated\n";
    }
    else
    {
        int i;
        for(i=n+1; i<siz; i++){
            if(free_list[i].size() != 0) break;
        }
        if(i == siz)
            cout << "Failed to allocate memory!!\n";
        else{
            pair<int, int> temp = free_list[i][0];
            free_list[i].erase(free_list[i].begin());
            i--;
            for(; i>=n; i--){
                pair<int, int> pair1, pair2;
                pair1 = make_pair(temp.first,
                    (temp.first + temp.second)/2);
                pair2 = make_pair((temp.first + temp.second + 1)/2,
                    temp.second);
                free_list[i].push_back(pair1);
                free_list[i].push_back(pair2);
                temp = free_list[i][0];
                free_list[i].erase(free_list[i].begin());
            }
            cout << "Memory from " << temp.first << " to "
              << temp.second << " allocated.\n";
            mp[temp.first] = temp.second - temp.first + 1;
        }
    }
}

void deallocate(int id){
    if(mp.find(id) == mp.end())
        cout << "Invalid Free Request\n";
    else{
        int n, buddy_number, buddy_add;
        n = ceil(log(mp[id])/log(2));
        int dest_id = id+pow(2,n)-1;
        free_list[n].push_back(make_pair(id, dest_id));
        cout << "Memory block from " << id << " to " <<
                dest_id << " free\n";
        buddy_number = id/mp[id];
        if(buddy_number % 2 == 0)
            buddy_add = id + pow(2,n);
        else
            buddy_add = id - pow(2,n);
        
        // Search in free_list to find it's buddy
        for(int i = 0; i < free_list[n].size(); i++){
            // If buddy found is also free
            if(free_list[n][i].first == buddy_add){
                // Merge smaller buddies to make them a larger one
                if(buddy_number % 2 == 0){
                    free_list[n+1].push_back(make_pair(id, id+(2*pow(2,n))-1));
                    cout << "Blocks starting at " << id << " and " << buddy_add << " merged!!\n"; 
                }
                else{
                    free_list[n+1].push_back(make_pair(buddy_add, buddy_add + (2*pow(2,n))-1));
                    cout << "Blocks starting at " << buddy_add << " and " << id << " merged!!\n"; 
                }
                free_list[n].erase(free_list[n].begin() + i);
                free_list[n].erase(free_list[n].begin() + free_list[n].size()-1);
                break;
            }
        }
        mp.erase(id);
    }
}

int main()
{
    int init;
    cout << "Enter space for initialization: ";
    cin >> init;
    initialize(init);
    cout << "\nType \"add size\" for allocate\nType \"remove size\" for deallocate\nType \"exit\" for exit\n";
    while(true){
        int space;
        string inp;
        cin >> inp;
        if(inp == "exit"){
            return 0;
        }
        if(inp == "add"){
            // cout << "Enter size of space to allocate: ";
            cin >> space;
            allocate(space);
        }
        else if(inp == "remove"){
            // cout << "Enter starting address to deallocate: ";
            cin >> space;
            deallocate(space);
        }
        
    }
    return 0;
}