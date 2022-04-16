#include <iostream>
#include <string>
using namespace std;   

class technique{
    protected:
        string name_device;
        int voltage; // volt
        int power_consumption; // Watt
        int dimensions[3]; // Meter
        int color;
        bool status; // on/off
        static int count_connect_device;
        static int power_consumption_in_system;
        technique(string _name, int _V, int _P, int _D[3], int _col) 
            : name_device(_name), voltage(_V), power_consumption(_P), color(_col){
            for(int i = 0; i < 3; i++) dimensions[i] = _D[i];
            status = 0;
        };
        technique(const technique & A){
            name_device = A.name_device;
            voltage = A.voltage;
            power_consumption = A.power_consumption;
            color = A.color;
            for(int i = 0; i < 3; i++) dimensions[i] = A.dimensions[i];
        };
    public:
        virtual void connect() = 0;
        virtual void disconnect() = 0;
        string get_name_device() const { return name_device; };
        int get_voltage() const { return voltage; };
        int get_power_consumption() const { return power_consumption; };
        int get_color() const { return color; };
        bool get_status() const { return status; };
        const int * get_dimensions() const { return dimensions; };
        virtual void print_device(ostream & os) = 0;
        friend ostream &  operator <<(ostream & os, technique & A);
        virtual ~technique(){};
};

int technique::count_connect_device = 0;
int technique::power_consumption_in_system = 0;

class microwave_oven : public technique{
    private:
        int volume_camera;
        int efficiency; // % //
        int heating_level; 
        int max_heating_level; 
        microwave_oven(string _name, int _V, int _P, int _D[3], int _col, int _vc, int _eff, int _hl, int _maxhl)
            : technique(_name, _V, _P, _D,_col), volume_camera(_vc), efficiency(_eff), 
            heating_level(_hl), max_heating_level(_maxhl)
            {};
    public:
        static microwave_oven include_microwave_oven();
        microwave_oven(const microwave_oven & M) : technique(M),
            volume_camera(M.volume_camera), efficiency(M.efficiency), heating_level(M.heating_level),
            max_heating_level(M.max_heating_level) {};
        virtual void connect(){
            count_connect_device++;
            power_consumption_in_system += power_consumption;
            status = 1;
            //... depend with device//
        };
        virtual void disconnect(){
            count_connect_device--;
            power_consumption_in_system -= power_consumption;
            status = 0;
            //... depend with device//

        };
        int turn_up_heating_level(int n = 1){
            heating_level += n;
            if(heating_level > max_heating_level) heating_level = max_heating_level;
            return heating_level;
        };
        int down_heating_level(int n = 1){
            heating_level -= n;
            if(heating_level < 1) heating_level = 1;
            return heating_level;
        };
        int get_efficiency() const { return efficiency; };
        int get_volume_camera() const { return volume_camera; };

        virtual void print_device(ostream & os){
            os << "Microwave oven" << endl;
            os << "name_device: " << get_name_device() << endl;
            os << "voltage: " << get_voltage() << endl;
            os << "power_consumption: " << get_power_consumption() << endl;
            os << "color: " << get_color() << endl;

            os << "demisions: " << get_dimensions()[0];
            os << " * "  << get_dimensions()[1];
            os << " * " << get_dimensions()[2] << endl;
            
            os << "volume_camera: " << get_volume_camera() << endl;
            os << "status: " << (get_status() ? "on" : "off") << endl;
            if(status) os << "current heating_level " <<  heating_level << endl; 
            os << endl;
        };

        microwave_oven & operator =(const microwave_oven & M) { 
            name_device = M.name_device;
            voltage = M.voltage; 
            power_consumption = M.power_consumption; 
            color = M.color;
            status = M.status; 
            count_connect_device = M.count_connect_device;
            power_consumption_in_system = M.power_consumption_in_system;
            for(int i = 0; i < 3; i++) dimensions[i] = M.dimensions[i];
            volume_camera = M.volume_camera;
            efficiency = M.efficiency;
            heating_level = M.heating_level;
            max_heating_level = M.max_heating_level; 
            return *this; 
        };

        virtual ~microwave_oven(){};
};

microwave_oven microwave_oven::include_microwave_oven(){
    string name;
    int V, P, D[3], col, vc, eff, hl = 1, maxhl;
    cout << "name_device: "; cin >> name;
    cout << "voltage: "; cin >> V;
    cout << "power_consumption: "; cin >> P;
    cout << "color: "; cin >> col;
    cout << "demisions: "; cin >> D[0] >> D[1] >> D[2];    
    cout << "volume_camera: "; cin >> vc;
    cout << "efficiency: "; cin >> eff;
    cout << "max_heating_level: "; cin >> maxhl;
    cout << "heating_level: "; cin >> hl;
    if( (hl > maxhl) || (hl < 0)) hl = 1;
    microwave_oven A(name, V, P, D, col, vc, eff, hl, maxhl);
    return A;
}; 

class washing_machine : public technique{
    private:
        int volume_camera;
        int c_mode;
        bool * mode;
        washing_machine(string _name, int _V, int _P, int _D[3], int _col, int _vc, int _cm, bool * _m)
            : technique(_name, _V, _P, _D,_col), volume_camera(_vc), c_mode(_cm), mode(_m)
            {};
    public:
        static washing_machine include_washing_machine();
        washing_machine(const washing_machine & M) : technique(M),
            volume_camera(M.volume_camera), c_mode(M.c_mode){
                mode = new bool[c_mode];
                for(int i = 0; i < c_mode; i++) mode[i] = M.mode[i];
            };
        virtual void connect(){
            count_connect_device++;
            power_consumption_in_system += power_consumption;
            status = 1;
            //... depend with device//
        };
        virtual void disconnect(){
            count_connect_device--;
            power_consumption_in_system -= power_consumption;
            status = 0;
            //... depend with device//

        };
        int get_volume_camera() const { return volume_camera; };
        void change_mode(int j){
            if(j > c_mode) cout << "incorrect value" << endl;
            else if(status != 0) mode[j - 1] = !mode[j - 1];
            else cout << "error, status: on" << endl;
        };
        virtual void print_device(ostream & os){
            os << "name_device: " << get_name_device() << endl;
            os << "washing_machine" << endl;
            os << "voltage: " << get_voltage() << endl;
            os << "power_consumption: " << get_power_consumption() << endl;
            os << "color: " << get_color() << endl;

            os << "demisions: " << get_dimensions()[0];
            os << " * "  << get_dimensions()[1];
            os << " * " << get_dimensions()[2] << endl;
            
            os << "volume_camera: " << get_volume_camera() << endl;
            os << "status: " << (get_status() ? "on" : "off") << endl;
            if(status){
                os << "current mode: "; 
                for(int i = 0; i < c_mode; i++) os << mode[i]; 
                os << endl;
            };
            os << endl;
        };

        washing_machine & operator =(const washing_machine & M){
            name_device = M.name_device;
            voltage = M.voltage; 
            power_consumption = M.power_consumption; 
            color = M.color;
            status = M.status; 
            count_connect_device = M.count_connect_device;
            power_consumption_in_system = M.power_consumption_in_system;
            for(int i = 0; i < 3; i++) dimensions[i] = M.dimensions[i];
            volume_camera = M.volume_camera;
            c_mode = M.c_mode;
            delete [] mode;
            mode = new bool[c_mode];
            for(int i = 0; i < c_mode; i++) mode[i] = M.mode[i];
            return *this;
        };

        virtual ~washing_machine(){ delete [] mode; };
};

washing_machine washing_machine::include_washing_machine(){
    string name;
    int V, P, D[3], col, vc, cm;
    bool * m;
    cout << "name_device: "; cin >> name;
    cout << "voltage: "; cin >> V;
    cout << "power_consumption: "; cin >> P;
    cout << "color: "; cin >> col;
    cout << "demisions: "; cin >> D[0] >> D[1] >> D[2];    
    cout << "volume_camera: "; cin >> vc;
    cout << "c_mode: "; cin >> cm;
    m = new bool[cm];
    cout << "mode: "; 
    for(int i = 0; i < cm; i++) cin >> m[i]; 
    washing_machine A(name, V, P, D, col, vc, cm, m);
    return A;
}; 

ostream &  operator <<(ostream & os, technique & A) { A.print_device(os); return os; };

int main(){

    microwave_oven A = microwave_oven::include_microwave_oven();
    cout << "\nDevice:" << endl;
    A.connect();
    cout << A;
    cout << "\nDevice:" << endl;
    A.disconnect();
    cout << A;

    washing_machine B = washing_machine::include_washing_machine();
    cout << "\nDevice:" << endl;
    B.connect();
    B.change_mode(2);
    cout << B;
    B.disconnect();
    cout << "\nchange:" << endl;
    B.change_mode(2);
    cout << "\nDevice:" << endl;
    cout << B;

    return 0;
}; 