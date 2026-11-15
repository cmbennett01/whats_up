// Name: settings.inc
// License: Public Domain
// Author: Christopher Bennett
// Version: 1.0, 2026-09-09
// Description: This file contains settings and utility functions for Stellarium scripts

// Define some colors
White = core.vec3f(1.0, 1.0, 1.0)                     // #FFFFFF
Gray20 = core.vec3f(0.8, 0.8, 0.8)                    // #CCCCCC
Gray40 = core.vec3f(0.6, 0.6, 0.6)                    // #999999  
Gray80 = core.vec3f(0.2, 0.2, 0.2)                    // #333333
Black = core.vec3f(0.0, 0.0, 0.0)                     // #000000

BrightRed = core.vec3f(0.8314, 0.1647, 0.1647)        // #D42A2A
BrightGreen = core.vec3f(0.00000, 0.56863, 0.00000)   // #009100
BrightCobalt = core.vec3f(0.00000, 0.28627, 0.56863)  // #004991
BrightPurple = core.vec3f(0.56863, 0.00000, 0.56863)  // #910091

DarkRed = core.vec3f(0.37647, 0.12549, 0.12941)       // #602021
DarkGreen = core.vec3f(0.17647, 0.28235, 0.17647)     // #2D482D
DarkSlate = core.vec3f(0.17647, 0.23137, 0.28235)     // #2D3B48
DarkPurple = core.vec3f(0.28235, 0.17647, 0.28235)    // #482D48

// Setup environment for the presentation
function setup() {
    core.debug("Setting up environment");

    // Az mount, atmosphere, landscape, no lines, labels or markers
    core.clear("natural");

    // Look West, fov 180°, zenith at the center of the view
    core.moveToAltAzi(90, 270, 3);
    StelMovementMgr.zoomTo(180, 3);

    // Clear all markers, labels, and images
    MarkerMgr.deleteAllMarkers();
    LabelMgr.deleteAllLabels();
    ScreenImageMgr.deleteAllImages();

    // Remove all sky images except the first two (default layers)
    var keys = StelSkyLayerMgr.getAllKeys();
    if (keys.length > 2) {
        for (var i = 2; i < keys.length; i++) {
            core.removeSkyImage(keys[i]);
        }
    }

    // show sky layer background images
    StelSkyLayerMgr.setFlagShow(true);

    // Set the observer location, landscape, and time
    core.setObserverLocation(loc.lon, loc.lat, loc.elev, loc.name, loc.planet);
    LandscapeMgr.setCurrentLandscapeName(landscape);
    core.setTimeRate(0); 
    core.setDate(dtg, spec = "local");

    core.setGuiVisible(false);
}

/**
 * Toggles the visibility of the azimuthal grid and dims the 
 * colors after a three second delay.
 */
function showAzGrid() {
    GridLinesMgr.setFlagAzimuthalGrid(false);

    GridLinesMgr.setColorAzimuthalGrid(BrightGreen);
    GridLinesMgr.setFlagAzimuthalGrid(true);
    core.wait(3);

    GridLinesMgr.setFlagAzimuthalGrid(false);
    core.wait(0.5);

    GridLinesMgr.setColorAzimuthalGrid(DarkGreen);
    GridLinesMgr.setFlagAzimuthalGrid(true);
}

/**
 * Toggles the visibility of the equatorial grid and dims the 
 * colors after a three second delay.
 */
function showEqGrid() {
    GridLinesMgr.setFlagEquatorGrid(false);

    GridLinesMgr.setColorEquatorGrid(BrightPurple);
    GridLinesMgr.setFlagEquatorGrid(true);
    core.wait(3);

    GridLinesMgr.setFlagEquatorGrid(false);
    core.wait(0.5);

    GridLinesMgr.setColorEquatorGrid(DarkPurple);
    GridLinesMgr.setFlagEquatorGrid(true);
}

/**
 * Fast forward at specified rate, until a specified time,
 * or until "L"key is pressed.
 */
function fastForward(rate, wait = "L", spec = "local") {
    StelMovementMgr.deselection();
    var timerate = core.getTimeRate();
    core.setTimeRate(rate);
    if (wait == "L") {
        L = LabelMgr.labelScreen("Rate = " + rate + (" press 'L' to resume"), 20, core.getScreenHeight() - 60,
            visible = true, fontSize = 18, fontColor = "#666666");
        i = 0;
        do {
            core.wait(1);
            i++;
        }
        while (core.getTimeRate() <= rate);
        core.setTimeRate(timerate);
        LabelMgr.deleteLabel(L);
    }
    else {
        core.waitFor(wait, spec = spec);
        core.setTimeRate(timerate);
    }
}

// Returns a date string from the Rise/Transit/Set of an object and current date
function dateFromRTS(timeString) {
    var date = core.getDate("local");      // "2026-06-29T14:23:51"
    date = date.substring(0, 10);          // "2026-06-29"

    // Convert "20h39m" -> "20:39:00"
    var time = timeString.replace("h", ":").replace("m", ":00");

    return date + "T" + time;
}

// Sets the time to sunrise on current date
function setSunrise() {
    var rts = core.getRTS("Sun");
    var date = dateFromRTS(rts["rise"]);
    core.setDate(date, "local");
}

// Sets the time to sunset on current date
function setSunset() {
    var rts = core.getRTS("Sun");
    var date = dateFromRTS(rts["set"]);
    core.setDate(date, "local");
}

// Sets the time to astronomical twilight (morning or evening) on current date
function setAstTwilight(which) {
    var rts = core.getRTS("Sun", -18);
    if (which === "start") {            //morning
        var date = dateFromRTS(rts["rise"]);
        core.setDate(date, "local");
    } else if (which === "end") {       //evening
        var date = dateFromRTS(rts["set"]);
        core.setDate(date, "local");
    }
}

// get sunrise on current date
function getAstSunrise() {
    var rts = core.getRTS("Sun");
    return dateFromRTS(rts["rise"]);
}

// get sunset on current date
function getSunset() {
    var rts = core.getRTS("Sun");
    return dateFromRTS(rts["set"]);
}

// get astronomical twilight (morning or evening) on current date
function getAstTwilight(which) {
    var rts = core.getRTS("Sun", -18);
    if (which === "start") {            //morning
        return dateFromRTS(rts["rise"]);
    } else if (which === "end") {       //evening
        return dateFromRTS(rts["set"]);
    }
}

/**
 * Loads a sky image layer using a parameter dictionary.
 * Wraps `core.loadSkyImage` and forwards fields from `params`.
 */
function loadImage(params) {
    core.loadSkyImage(
        params.id,
        params.path,
        params.ra,
        params.dec,
        params.angSize,
        params.rotation,
        params.minRes,
        params.maxBright,
        params.visible
    );
}


// Draws pointer line along a great circle between two RA/Dec coordinates (in degrees)
function drawPointer(obj1Name, obj2Name, steps, colorHex) {
    var obj1 = core.getObjectInfo(obj1Name);
    var ra1Deg = obj1["ra"];
    var dec1Deg = obj1["dec"];

    var obj2 = core.getObjectInfo(obj2Name);
    var ra2Deg = obj2["ra"];
    var dec2Deg = obj2["dec"];

    var deg2rad = Math.PI / 180.0;
    var rad2deg = 180.0 / Math.PI;

    // 1. Convert start and end RA/Dec to 3D unit vectors
    var ra1 = ra1Deg * deg2rad;
    var dec1 = dec1Deg * deg2rad;
    var x1 = Math.cos(dec1) * Math.cos(ra1);
    var y1 = Math.cos(dec1) * Math.sin(ra1);
    var z1 = Math.sin(dec1);

    var ra2 = ra2Deg * deg2rad;
    var dec2 = dec2Deg * deg2rad;
    var x2 = Math.cos(dec2) * Math.cos(ra2);
    var y2 = Math.cos(dec2) * Math.sin(ra2);
    var z2 = Math.sin(dec2);

    // 2. Calculate angular distance (omega) between the two points
    var dot = x1 * x2 + y1 * y2 + z1 * z2;
    dot = Math.max(-1.0, Math.min(1.0, dot)); // Clamp for floating-point safety
    var omega = Math.acos(dot);
    var sinOmega = Math.sin(omega);

    // 3. Interpolate along the great circle
    for (var i = 0; i <= steps; i++) {
        var t = i / steps;
        var w1, w2;

        if (sinOmega < 1e-6) {
            // Fallback for points very close together
            w1 = 1.0 - t;
            w2 = t;
        } else {
            w1 = Math.sin((1.0 - t) * omega) / sinOmega;
            w2 = Math.sin(t * omega) / sinOmega;
        }

        var x = w1 * x1 + w2 * x2;
        var y = w1 * y1 + w2 * y2;
        var z = w1 * z1 + w2 * z2;

        // 4. Convert interpolated 3D vector back to RA/Dec degrees
        var dec = Math.asin(Math.max(-1.0, Math.min(1.0, z))) * rad2deg;
        var ra = Math.atan2(y, x) * rad2deg;
        if (ra < 0) {
            ra += 360.0;
        }

        MarkerMgr.markerEquatorial(ra + "d", dec + "d", true, true, "disk", colorHex, 3.0);
        core.wait(0.05); // Optional: small delay for animated visual effect
    }
}

// select constellation by name and display isolated (lines and art)
function showConstellation(Const) {

    ConstellationMgr.deselectConstellations();
    ConstellationMgr.setFlagIsolateSelected(true);
    ConstellationMgr.setFlagConstellationPick(true);

    core.selectConstellationByName(Const);
    ConstellationMgr.setFlagLines(true);
    ConstellationMgr.setFlagArt(true);
}

// move to a selected object, zoom in and back out
function goToAltAz(obj, fov=1, duration=4) {

    // Move to object, initially zoomed out
    core.moveToAltAzi(90, obj["azimuth"], duration);
    StelMovementMgr.zoomTo(180, duration);
    core.wait(duration);
    core.waitForKeypress();

    // Zoom in (centered 1/5 of the specified field of view above the object)
    core.moveToAltAzi(obj["altitude"] + fov/5, obj["azimuth"], duration);
    StelMovementMgr.zoomTo(fov, duration);
    core.wait(duration);
    ConstellationMgr.setFlagLines(false);
    ConstellationMgr.setFlagArt(false);
    core.waitForKeypress();

    // Zoom out
    core.moveToAltAzi(90, obj["azimuth"], duration);
    StelMovementMgr.zoomTo(180, duration);
    core.wait(duration);
}

// wrapper function to go to an object, optionally showing its constellation
function objectGoTo(name, fov=1, duration=4, Const=true) {
    var obj = core.getObjectInfo(name);

    if (Const) {
        showConstellation(obj["iauConstellation"]);
    } else {
        ConstellationMgr.deselectConstellations();
        ConstellationMgr.setFlagLines(false);
        ConstellationMgr.setFlagArt(false);
    }

    goToAltAz(obj, fov, duration);
}
