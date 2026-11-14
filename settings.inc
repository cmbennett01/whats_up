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

    // reset view to home.
    core.goHome();

    // az mount, atmosphere, landscape, no lines, labels or markers
    core.clear("natural");

    // clear all markers, labels, and images
    StelSkyLayerMgr.setFlagShow(false);

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

    // Set the observer location and time
    core.setObserverLocation(loc.lon, loc.lat, loc.elev, loc.name, loc.planet);
    core.setTimeRate(0); 
    core.setDate(dtg, spec = "local");

    core.setGuiVisible(false);
}

// Setup the environment for constellations tour
function setupConstellations() {
    // eq mount, const lines, no landscape, atmosphere, grid lines, labels or markers.
    core.clear("deepspace");

    ConstellationMgr.setFontSize(18); // default size is 15
    ConstellationMgr.setArtIntensity(0.2);
    ConstellationMgr.setLinesColor(Gray40);
    ConstellationMgr.setLabelsColor(Gray40);
    ConstellationMgr.setBoundariesColor(Gray80);
    ConstellationMgr.setConstellationLineThickness(1);
    ConstellationMgr.setFlagIsolateSelected(true);
    ConstellationMgr.setFlagConstellationPick(true);
    ConstellationMgr.deselectConstellations();

    ConstellationMgr.setFlagLines(true);
    ConstellationMgr.setFlagArt(true);
    ConstellationMgr.setFlagBoundaries(true);
}

/**
 * Cleans up the application state by restoring the previously saved state.
 * It also: 
 * - Manually resets the Solar System and Star font size (default 13).
 * - Deletes all markers and labels.
 * - Makes the GUI visible.
 */
function cleanup() {
    restoreState("RestoreState");

    // There is no getter for SolarSystem and StarMgr font size, restore them manually.
    StarMgr.setFontSize(13); // default size is 13
    SolarSystem.setFontSize(13); // default size is 13

    MarkerMgr.deleteAllMarkers();
    LabelMgr.deleteAllLabels();

    ScreenImageMgr.deleteAllImages();

    var keys = StelSkyLayerMgr.getAllKeys();
    core.debug(StelSkyLayerMgr.getAllKeys());

    if (keys.length > 2) {
        for (var i = 2; i < keys.length; i++) {
            core.removeSkyImage(keys[i]);
        };
    };

    core.setGuiVisible(true);
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

// Fast forwards the time rate until a specified time or unt26il key is pressed.
function fastForward(rate, wait = "L", spec = "local") {
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

// Sets the time based on the Rise/Transit/Set of an object
function setTimeFromRTS(timeString) {
    var date = core.getDate("local");      // "2026-06-29T14:23:51"
    date = date.substring(0, 10);          // "2026-06-29"

    // Convert "20h39m" -> "20:39:00"
    var time = timeString.replace("h", ":").replace("m", ":00");

    core.setDate(date + "T" + time, "local");
}

// Sets the time to sunrise
function setSunrise() {
    var rts = core.getRTS("Sun");
    setTimeFromRTS(rts["rise"]);
}

// Sets the time to sunset
function setSunset() {
    var rts = core.getRTS("Sun");
    setTimeFromRTS(rts["set"]);
}

// Sets the time to astronomical twilight (morning or evening)
function setAstTwilight(which) {
    var rts = core.getRTS("Sun", -18);
    if (which === "start") {            //morning
        setTimeFromRTS(rts["rise"]);
    } else if (which === "end") {       //evening
        setTimeFromRTS(rts["set"]);
    }
}

// get astronomical twilight (morning or evening)
function getAstTwilight(which) {
    var date = core.getDate("local");   // "2026-06-29T14:23:51"
    date = date.substring(0, 10);          // "2026-06-29"
    var rts = core.getRTS("Sun", -18);

    if (which === "start") {            //morning
        var time = rts["rise"].replace("h", ":").replace("m", ":00");
        return date + "T" + time;
    } else if (which === "end") {       //evening
        var time = rts["set"].replace("h", ":").replace("m", ":00");
        return date + "T" + time;
    }
}

/**
 * Loads a sky image layer using a parameter dictionary.
 * Wraps `core.loadSkyImage` and forwards fields from `params`.
 *
 * Params:
 * - params.id: string — unique layer identifier.
 * - params.path: string — image path (relative or absolute).
 * - params.ra: string — RA center (e.g., "14h 29m 29.2s").
 * - params.dec: string — Dec center (e.g., "-62d 40m 25s").
 * - params.angSize: number — angular size in degrees.
 * - params.rotation: number — rotation angle in degrees.
 * - params.minRes: number — minimum resolution threshold.
 * - params.maxBright: number — maximum brightness scaling.
 * - params.visible: boolean — show layer immediately when loaded.
 *
 * Example usage:
 *  loadImage(proxima);
 * 
 * Note: Image params are in images.inc in root scripts folder.
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
function drawPointer(ra1Deg, dec1Deg, ra2Deg, dec2Deg, steps, colorHex) {
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


