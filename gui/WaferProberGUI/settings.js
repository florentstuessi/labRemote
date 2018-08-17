// settings.js
.pragma library

var xy_device = "/dev/ttyACM0"


// default speed [mm/s]
var speed_x = 2
var speed_y = 2
var speed_z = 0.1

// default increment (stepping interval)
var incre_x = 28.7998
var incre_y = 23.6


// size of images
var image_width = 640
var image_height = 480
var margin = 5

// hard coded path that houses captured images
// var image_saved_path = "/Users/xju/Documents/2017/RD53/code/labRemote/gui/WaferProberGUI/captured_image/"
var image_saved_path = "/home/pixel/Documents/probing_station/code/labRemote/gui/WaferProberGUI/captured_image/"


var add =  (
    function () {
        var counter = 0;
        return function() {return counter += 1;}
    }
)();

// a table that stores a ideal locations for each chip,
// based on the provided location of any chip.
// The table will be generate dynamically
// by the function "update_true_chip_table"
var true_chip_table = {};


var chip_id_for_calibration = "1-6"
var chip_x_for_calibration = 290.441
var chip_y_for_calibration = 139.805

// a 10 columns and 12 rows
// x = 10, y = 12
// these numbers are chip_ID
var chip_numbering = [
            [-1, -1, -1, -1, 33, 45, -1, -1, -1, -1],
            [-1, -1, 12, 22, 34, 46, 57, 68, -1, -1],
            [-1, 4,  13, 23, 35, 47, 58, 69, 78, -1],
            [-1, 5,  14, 24, 36, 48, 59, 70, 79, -1],
            [-1, 6,  15, 25, 37, 49, 60, 71, 80, 86],
            [1,  7,  16, 26, 38, 50, 61, 72, 81, 87],
            [2,  8,  17, 27, 39, 51, 62, 73, 82, 88],
            [3,  9,  18, 28, 40, 52, 63, 74, 83, 89],
            [-1, 10, 19, 29, 41, 53, 64, 75, 84, 90],
            [-1, 11, 20, 30, 42, 54, 65, 76, 85, -1],
            [-1, -1, 21, 31, 43, 55, 66, 77, -1, -1],
            [-1, -1, -1, 32, 44, 56, 67, -1, -1, -1]

        ];


var find_location = function(id_){
    for(var i = 0; i < 12; i++){
        for(var j = 0; j < 10; j++){
            if(chip_numbering[i][j] == id_){
                return {x_loc: j, y_loc: i};
            }
        }
    }
    return {x_loc: 0, y_loc: 0};
}

var find_chip_number = function(chip_name) {
    // console.log("HELLO: ", id_)
    var items = chip_name.split("-");
    var x = Number(items[0]) - 1;
    var y = Number(items[1]) - 1;
    // console.log("location from chipID:", id_, x, y, chip_numbering[y][x])
    return chip_numbering[y][x];
}

var update_true_chip_table = function(id_, x_, y_) {
    var input_loc = find_location(id_);

    var x_zero = x_ + incre_x * input_loc.x_loc;
    var y_zero = y_ - incre_y * input_loc.y_loc;
    var n_properties = 0
    for(var j = 0; j < 10; j ++) {
        // a table that stores a height different between the first chip(1-6) and other chips.
        var height_table = {
            // input_name: "",
            input_name: "/home/pixel/Documents/probing_station/code/labRemote/gui/WaferProberGUI/height_table.txt",
            table: {},
            refID: "1-6",
            read: function (input_text) {
                var lines = input_text.split('\n')
                for(var line_nb in lines){
                    var items = lines[line_nb].split(' ');
                    this.update(items[0], items[1])
                }
                console.log(this.input_name+" is loaded with "+lines.length+" lines")
            },
            output: function() {
                var out = "";
                for(var item in this.table) {
                    var locs = this.table[item]
                    var input_loc = find_location(Number(item));
                    var res = (input_loc.x_loc+1)+"-"+(input_loc.y_loc+1)
                    out += res + " " + locs.deltaZ + "\n"
                }
                return out
            },
            update: function(id_input, z_){
                // id_: 1-6
                // z_: 0.1
                var id_ = find_chip_number(id_input)
                if(this.table[id_.toString()] === undefined){
                    this.table[id_.toString()] = {
                        deltaZ: z_
                    }
                } else {
                    this.table[id_.toString()].deltaZ = z_
                }
                console.log("Height table is updated: ", id_input, z_)
            },
            get_origin_height: function(id_input) {
                var id_ = find_chip_number(id_input)
                var z_height = this.table[id_.toString()]
                if(z_height == undefined) {
                    return 0.
                } else {
                    return z_height
                }
            },
            get: function(id_input) {
                var cur_height = this.get_origin_height(id_input).deltaZ
                var ref_height = this.get_origin_height(this.refID).deltaZ
                var diff = (cur_height - ref_height)
                return diff
            }
        }
        for(var i = 0; i < 12; i++){
            var chip_id = chip_numbering[i][j];
            if(chip_id < 0) continue;
            true_chip_table[chip_id.toString()] = {
                xAxis: (x_zero - j * incre_x) * 1000 / 1000,
                yAxis: (y_zero + i * incre_y) * 1000/ 1000
            }
            n_properties += 1
        }
    }
    console.log(n_properties," properties are updated for true chip Table!")
}

// return x,y axis for required chip_id
var get_chip_axis = function(chip_id) {
    if( typeof chip_id == "number") {
        // valid input, do something.
        return true_chip_table[chip_id.toString()];
    } else {
        return true_chip_table["0"];
    }
}

var convert_ID_to_name = function(chip_id) {
    var input_loc = find_location(chip_id);
    var res = (input_loc.x_loc+1)+"-"+(input_loc.y_loc+1)
    return res;
}

// find nearest chip name given current location
var find_chip_name = function(x_, y_){
    var distance = 999999;
    var closet_chip = -1;
    for(var item in true_chip_table){
        if(true_chip_table.hasOwnProperty(item)) {
            var chip_x = true_chip_table[item].xAxis;
            var chip_y = true_chip_table[item].yAxis;
            var cur_dis = Math.sqrt(Math.pow((x_ - chip_x), 2) + Math.pow((y_ - chip_y), 2));
            if(cur_dis < distance) {
                closet_chip = item;
                distance = cur_dis
            }
        }
    }
    return convert_ID_to_name(Number(closet_chip));
}

// a table that stores a correct hand-selected locations
// for each chip.
// read from a text file: read_chip_table()
// after finishing calibration, write the text file: write_chip_table()
// update each chip with: update_chip_table()

var real_chip_table = {
    input_name: "/home/pixel/Documents/probing_station/code/labRemote/gui/WaferProberGUI/real_chip_table.txt",
    table: {},
    read: function (input_text) {
        var lines = input_text.split('\n')
        for(var line_nb in lines){
            var items = lines[line_nb].split(' ');
            this.updateWithArray(items)
        }
        console.log(this.input_name+" is read.")
    },
    output: function() {
        var out = "";
        for(var item in this.table) {
            var locs = this.table[item]
            out += item+" " + locs.xAxis + " " + locs.yAxis + "\n"
        }
        return out
    },
    update: function(id_, x_, y_){
        if(this.table[id_.toString()] === undefined){
            this.table[id_.toString()] = {
                xAxis: x_,
                yAxis: y_
            }
        } else {
            this.table[id_.toString()].xAxis = x_
            this.table[id_.toString()].yAxis = y_
        }
    },
    updateWithArray: function(items) {
        if (items.length < 3) return;
        this.update(Number(items[0]), Number(items[1]), Number(items[2]))
    }
}

// a table that stores a height different between the first chip(1-6) and other chips.
var height_table = {
    // input_name: "",
    input_name: "/home/pixel/Documents/probing_station/code/labRemote/gui/WaferProberGUI/height_table.txt",
    table: {},
    refID: "1-6",
    read: function (input_text) {
        var lines = input_text.split('\n')
        for(var line_nb in lines){
            var items = lines[line_nb].split(' ');
            this.update(items[0], items[1])
        }
        console.log(this.input_name+" is loaded with "+lines.length+" lines")
    },
    output: function() {
        var out = "";
        for(var item in this.table) {
            var locs = this.table[item]
            var input_loc = find_location(Number(item));
            var res = (input_loc.x_loc+1)+"-"+(input_loc.y_loc+1)
            out += res + " " + locs.deltaZ + "\n"
        }
        return out
    },
    update: function(id_input, z_){
        // id_: 1-6
        // z_: 0.1
        var id_ = find_chip_number(id_input)
        if(this.table[id_.toString()] === undefined){
            this.table[id_.toString()] = {
                deltaZ: z_
            }
        } else {
            this.table[id_.toString()].deltaZ = z_
        }
        console.log("Height table is updated: ", id_input, z_)
    },
    get_origin_height: function(id_input) {
        var id_ = find_chip_number(id_input)
        var z_height = this.table[id_.toString()]
        if(z_height == undefined) {
            return 0.
        } else {
            return z_height
        }
    },
    get: function(id_input) {
        var cur_height = this.get_origin_height(id_input).deltaZ
        var ref_height = this.get_origin_height(this.refID).deltaZ
        var diff = (cur_height - ref_height)
        return diff
    }
}
