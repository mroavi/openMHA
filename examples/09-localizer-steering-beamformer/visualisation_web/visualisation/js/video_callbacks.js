"use strict";
// This file is part of the HörTech Open Master Hearing Aid (openMHA)
// Copyright © 2018 HörTech gGmbH
//
// openMHA is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// openMHA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License, version 3 for more details.
//
// You should have received a copy of the GNU Affero General Public License, 
// version 3 along with openMHA.  If not, see <http://www.gnu.org/licenses/>.

function set_angle_width() {
    var angle_width = parseFloat(d3.select('#anglewidth').node().value);
    var video = d3.select('video');
    var video_node = video.node();

    // Adapt the video width to the specified angle width.
    d3.select('#anglewidth_val').text(angle_width);
    video_node.width = x(angle_width/2) - x(-angle_width/2);

    /*
     * Adapt margin-top so that the webcam image is centered .  This works by
     * computing the difference between the actual video height and the height
     * of the video's <div> container, then dividing that by half, which yields
     * the amount that the image needs to be shifted upwards.
     */
    var container_height = parseInt(d3.select('#video-container').style('height'));
    var width_ratio = video_node.width/video_node.videoWidth;
    var video_height = video_node.videoHeight*width_ratio;
    var new_margin = -(video_height - container_height)/2;

    /* Align the video with the bottom of the plot if it fits completely inside
     * the containing <div>. */
    if ( video_height < container_height ) {
        new_margin *= 2;
    }

    video.style('margin-top', new_margin + 'px');
}

function toggle_video_flipping() {
    var video = d3.select('video');
    var flip_video = d3.select('#flipvideo').node().checked;

    if ( flip_video ) {
        video.style('transform', 'scaleX(-1)');
    } else {
        video.style('transform', '');
    }
}

// Functions that should be called by the .onopen() callback of a WebSocket
// object, which should be done in ui_callbacks.js, which should by used by all
// variations of the UI.
function video_onopen() {
    set_angle_width();
    toggle_video_flipping();
}
