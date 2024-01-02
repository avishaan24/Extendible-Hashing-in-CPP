#include<bits/stdc++.h>
#define IOS ios_base::sync_with_stdio(0);cin.tie(NULL);cout.tie(NULL);
using namespace std;
#define ll long long
#define fl(x,y,n) for(ll x=y;x<n;x++)
#define rfl(x,y,n) for(ll x=y;x>=n;x--)
#define line "\n"
#define yesno  if(result) cout<<"YES"<<line;else cout<<"NO"<<line;
#define pb push_back
#define all(v) v.begin(),v.end()
#define rev(v) v.rbegin(),v.rend()
#define mod  1000000007

// Bucket class which is used to store data
class Bucket{
    public:
        ll local_depth, bucket_size;
        multiset<ll>box;

        Bucket(ll local_depth, ll bucket_size);

        bool insertion(ll insert);

        void deletion(ll val);

        void print_bucket();
};

// Directory class which is used to hash
class Extendible_Hashing{
    private:
        ll bucket_size, global_depth, local_depth, change, index;
        map<ll, Bucket*>directory;
        Bucket *temp, *del;
        multiset<ll>elements;
    public:
        Extendible_Hashing(ll bucket_size);

        ll get_index(ll val, ll depth);

        string get_binary(ll val);

        void increase_global_depth();

        void increase_local_depth();

        void remove_overflow(ll val);

        void insertion(ll val);

        void merge_bucket(vector<ll> &changes);

        void decrease_global_depth();

        void deletion(ll val);

        void print_directory();
};

// All bucket class function definition

// Constructor function which is used to construct Bucket class
Bucket::Bucket(ll local_depth, ll bucket_size){
    this->local_depth = local_depth;
    this->bucket_size = bucket_size;
    box.clear();
}

// insertion function which is used to insert data in the bucket
bool Bucket::insertion(ll insert){
    if(bucket_size == box.size())
        return false; // bucktet is full so we have to expand bucket and remove overflow
    else{
        box.insert(insert);
        return true;
    }
}

// deletion function which is used to delete data from the bucket
void Bucket::deletion(ll val){
    box.erase(box.find(val));
}

// print_bucket function used to print the data store in the bucket
void Bucket::print_bucket(){
    for(auto &bx:box)
        cout << bx << " ";
    if(box.size() == 0){
        cout << "Empty!  ";
    }
    cout << "(Local Depth: "<< local_depth << ")";
    cout << line;
}

// All function decalaration of Extendible_Hashing function

// Constructor function to declare the directory for hashing
Extendible_Hashing::Extendible_Hashing(ll bucket_size){
    this->bucket_size = bucket_size;
    this->global_depth = 0;
}

// function to calculate the value in binary till depth
ll Extendible_Hashing::get_index(ll val, ll depth){
    ll ct = 0;
    // calculate decimal number taking last depth bit
    fl(i,0,depth)
        if((val >> i) & 1)
            ct += (1 << i);
    return ct;
}

// function to convert number to binary
string Extendible_Hashing::get_binary(ll val){
    string s = "";
    rfl(i, global_depth - 1, 0)
        if((val >> i) & 1)
            s += '1';
        else
            s += '0';
    return s;
}

// function used to increase the global depth of the directory structure
void Extendible_Hashing::increase_global_depth(){
    global_depth++;
    directory[change]->local_depth++;
    fl(j, (1 << (global_depth - 1)), (1 << (global_depth))){
        // bucket which have to be expand 
        if(j == (change + (1 << (global_depth - 1))))
            directory[j] = new Bucket(directory[change]->local_depth,bucket_size);
        // else pointing to the same bucket of same local depth
        else
            directory[j] = directory[j - (1 << (global_depth - 1))];
    } 
    cout << "Global depth increased to: " << global_depth << endl;
}

// function used to increase the local depth of the bucket
void Extendible_Hashing::increase_local_depth(){
    directory[index]->local_depth++;
    bool result = true;
    ll lst = -1;
    fl(i, 0, (1 << global_depth)){
        if(get_index(i, directory[index]->local_depth) == change + (1 << (directory[index]->local_depth - 1))){
            if(result){
                directory[i] = new Bucket(directory[index]->local_depth, bucket_size);
                lst = i;
                result = false;
            }
            else
                directory[i] = directory[lst];
        }
    }
}

// function used to handle overflow while inserting a new data
void Extendible_Hashing::remove_overflow(ll val){
    multiset<ll>extra = directory[index]->box;
    directory[index]->box.clear();
    extra.insert(val);
    for(auto &ex:extra)
        elements.erase(elements.find(ex));
    change = index;
    if(global_depth == directory[index]->local_depth)
        increase_global_depth();
    else{
        change = get_index(val, directory[index]->local_depth);
        increase_local_depth();
    }
    for(auto &ex:extra)
        insertion(ex);
}

// function used to insert data into the directory structure
void Extendible_Hashing::insertion(ll val){
    elements.insert(val);
    index = get_index(val, global_depth);
    if(global_depth == 0){
        if(directory.find(0) == directory.end())
            directory[0] = new Bucket(0,bucket_size);
    }
    if(!directory[index]->insertion(val))
        remove_overflow(val);
}

// function used to merge two bucket into one bucket and decrease local depth
void Extendible_Hashing::merge_bucket(vector<ll> &changes){
    multiset<ll>extra = del->box;
    free(del);
    fl(i,0,changes.size())
        directory[changes[i]] = temp;
    for(auto &ex:extra){
        elements.erase(elements.find(ex));
        insertion(ex);
    }
    temp->local_depth--;
    local_depth--;
}

// function used to decrease the global depth of the directory structure
void Extendible_Hashing::decrease_global_depth(){
    fl(i,(1 << (global_depth - 1)), 1 << global_depth)
        directory.erase(i);
    global_depth--;
    cout << "Global depth decreased to: " << global_depth << endl;
}

// function used to delete data from the directory structure
void Extendible_Hashing::deletion(ll val){
    if(elements.find(val) == elements.end())
        cout << "Element not found!" << line;

    else{
        elements.erase(elements.find(val));
        index = get_index(val, global_depth);
        directory[index]->deletion(val);
        local_depth = directory[index]->local_depth;
        temp = directory[index], del = NULL;
        bool result = local_depth > 1;

        while(result){
            set<Bucket*>same_bucket;
            same_bucket.insert(temp);
            index = get_index(val, local_depth - 1);
            vector<ll>changes;
            fl(i,0,(1 << global_depth)){
                if(get_index(i, local_depth - 1) == index && directory[i]->local_depth == temp->local_depth){
                    same_bucket.insert(directory[i]);
                    changes.pb(i);
                }
            }
            // find two merging buckets
            for(auto &it:same_bucket)
                if(it != temp)
                    del = it;
            // if data stored in the two bucket is greater than bucket size, no possibility of merging
            if(same_bucket.size() == 1 || (del && del->box.size() + temp->box.size() > bucket_size))
                result = false;
            // else merge the two buckets
            else
                merge_bucket(changes);
            if(local_depth <= 1)
                result = false;
        }

        // if total element is less than bucket size then one bucket is required
        if(elements.size() <= bucket_size){
            set<Bucket*>buck;
            fl(i, 0, (1 << global_depth))
                buck.insert(directory[i]);
            for(auto &it:buck)
                free(it);
            directory.clear();
            global_depth = 0;
            multiset<ll>extra = elements;
            elements.clear();
            for(auto &ex:extra)
                insertion(ex);
            cout << "Global depth decreased to: " << global_depth << endl;
        }

        // checking condition to decrease the global depth
        if(elements.size() && global_depth){
            bool result = true;
            while(result){
                fl(i,0,(1 << global_depth))
                    result &= directory[i]->local_depth < global_depth;
                if(result)
                    decrease_global_depth();
            }
        }
    }
}

// function used to print all the data stored in the directory
void Extendible_Hashing::print_directory(){
    map<Bucket*,vector<string>>print_dir;
    cout << "Global depth: " << global_depth << line << line;
    if(elements.size()){
        fl(i,0,(1 << global_depth))
            print_dir[directory[i]].pb(get_binary(i));
    }
    if(global_depth){
        ll sz = 0;
        for(auto &it:print_dir)
            sz = max(sz,(ll)print_dir[it.first].size());
        sz = sz * global_depth + (sz - 1) * 2 + 5;
        for(auto &it:print_dir){
            fl(i,0,(sz + 42)/2)
                cout << "- ";
            cout << line;
            ll occu = print_dir[it.first].size() * global_depth + (print_dir[it.first].size() - 1) * 2;
            fl(i,0,print_dir[it.first].size()){
                if(i != print_dir[it.first].size() - 1)
                    cout << print_dir[it.first][i] << ", ";
                else
                    cout << print_dir[it.first][i];
            }
            fl(i,occu + 1,sz)
                cout << " ";
            cout << "======>   ";
            it.first->print_bucket();
        }
        fl(i,0,(sz + 42)/2)
            cout << "- ";
        cout << line;
    }
    else{
        cout << "..  ========> ";
        if(elements.size())
            print_dir.begin()->first->print_bucket();
        else
            cout << "Empty!" << line;
    }
}

int main(){
    bool result = true;
    ll bucket_size, operation, val;
    cout << "Enter bucket size : ";
    cin >> bucket_size;
    // creating new directory of bucket size as bucket_size
    Extendible_Hashing hsh(bucket_size); 
    do{
        cout << "Insertion(1) \t Deletion(2) \t Show Directory(3) \t Exit(4) : ";
        cin >> operation;
        cout << line;
        if(operation == 1){
            cout << "Enter inserting value : ";
            cin >> val;
            hsh.insertion(val);
        }
        else if(operation == 2){
            cout << "Enter deletion value : ";
            cin >> val;
            hsh.deletion(val);
        }
        else if(operation == 3)
            hsh.print_directory();
        else{
            cout << "Thank You! \nBye, See you soon!";
            result = false;
        }
        cout << line;
    } while (result);
    return 0;
}