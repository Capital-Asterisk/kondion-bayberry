/**
 * @file Masterscript, this script is run first
 * @author Neal Nicdao (Capital_Asterisk)
 */

kdion.initialize(function() {
	
	OKO_Camera_ = OKO_Camera;
	
	kdion.log("Hello world");
	kdion.Bird();
	var f = new kdion.Bird();
	kdion.log("Bird: " + f);
	kdion.log("Bird is crushed: " + f.crushed);
	kdion.log("Bird integrity: " + f.integrity);
	kdion.log("Crushing bird...");
	f.crushed = true;
	f.integrity = 5.5;
	kdion.log("Bird is crushed: " + f.crushed);
	kdion.log("Bird is integrity: " + f.integrity);
	var c = new kdion.Chicken();
	kdion.log("Bird: " + c);
	kdion.log("Bird is crushed: " + c.crushed);
	kdion.log("Bird integrity: " + c.integrity);
	kdion.log("Crushing bird...");
	c.crushed = true;
	c.integrity = 5.5;
	kdion.log("Bird is crushed: " + c.crushed);
	kdion.log("Bird is integrity: " + c.integrity);
	kdion.log("World: " + kdion.World)
	var e = new KObj_Entity();
	e.translate(0.0, 0.5, 0.0);
	kdion.e = e;
	
	var drag = new KObj_Entity();
	drag.translate(0.0, 1.5, 0.4);
	drag.setParent(kdion.World);
	
	var camera = new OKO_Camera();
	
	camera.translate(0.0, 2.0, 6.0);
	//kdion.log(kdion.camera);
	
	camera.setParent(kdion.World);
	e.setParent(kdion.World, "hey theretttt", 5, 5, 8);
	kdion.load({textures: ["tiles_diff", "tiles_norm"]});
	kdion.load({textures: 5});
	kdion.camera = camera;
});

kdion.globalUpdate(function() {
	//kdion.log("");
	//kdion.log(kdion.input["MOUSE_X"]);
	//kdion.log(new Date().getTime());
	kdion.camera.pointAt(kdion.e);
	kdion.e.translate(-kdion.input["MOVE_X"] * 0.06, 0.0, kdion.input["MOVE_Y"] * 0.06);
	//kdion.log(kdion.camera);
	//kdion.camera.translate(0.0, 0.01, kdion.input["MOVE_Y"] * 0.03);
	
});

