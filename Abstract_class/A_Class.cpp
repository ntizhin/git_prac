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
        virtual void print_device() = 0;
        virtual ~technique(){};
};

int technique::count_connect_device = 0;
int technique::power_consumption_in_system = 0;

class microwave_oven : protected technique{
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
        virtual void print_device(){
             cout << "Microwave oven" << endl;
            cout << "name_device: " << get_name_device() << endl;
            cout << "voltage: " << get_voltage() << endl;
            cout << "power_consumption: " << get_power_consumption() << endl;
            cout << "color: " << get_color() << endl;

            cout << "demisions: " << get_dimensions()[0];
            cout << " * "  << get_dimensions()[1];
            cout << " * " << get_dimensions()[2] << endl;
            
            cout << "volume_camera: " << get_volume_camera() << endl;
            cout << "status: " << (get_status() ? "on" : "off") << endl;
            if(status) cout << "current heating_level " <<  heating_level << endl; 
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

int main(){
    microwave_oven A = microwave_oven::include_microwave_oven();
    cout << "\nDevice:" << endl;
    A.connect();
    A.print_device();
    cout << "\nDevice:" << endl;
    A.disconnect();
    A.print_device();
    return 0;
}; 