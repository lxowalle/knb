#include "thr_pjsip.h"

#define THIS_FILE __FILE__

static pj_status_t app_init(void)
{
    pjsua_config ua_cfg;
    pjsua_logging_config log_cfg;
    pjsua_media_config media_cfg;
    pj_status_t status;

    // Must create pjsua before anything else!
    status = pjsua_create();
    if (status != PJ_SUCCESS) 
    {
        pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
        return status;
    }

    // Initialize configs with default settings.
    pjsua_config_default(&ua_cfg);
    pjsua_logging_config_default(&log_cfg);
    pjsua_media_config_default(&media_cfg);
    
    // At the very least, application would want to override
    // the call callbacks in pjsua_config:
    // ua_cfg.cb.on_incoming_call = ...
    // ua_cfg.cb.on_call_state = ...

    // Customize other settings (or initialize them from application specific
    // configuration file):

    // Initialize pjsua
    status = pjsua_init(&ua_cfg, &log_cfg, &media_cfg);
    if (status != PJ_SUCCESS) 
    {
        pjsua_perror(THIS_FILE, "Error initializing pjsua", status);
        return status;
    }
}

static pj_status_t app_cfg_init(void)
{
    pj_status_t res = PJ_SUCCESS;

    pjsua_transport_config transport_cfg;
    pjsua_transport_id transport_id = -1;
    pjsua_transport_config_default(&transport_cfg);

    res = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, &transport_id);
    if (res != PJ_SUCCESS)
    {
        pjsua_perror(THIS_FILE, "Error create transport", res);
        return res;
    }


}

static pj_status_t app_deinit(void)
{
    pj_status_t res = PJ_SUCCESS;

    res = pjsua_destroy();
    if (res != PJ_SUCCESS)
    {
        pjsua_perror(THIS_FILE, "Error deinit pjsua", res);
        return res;
    }

    return res;
}

static pj_status_t app_run(void)
{
    // Run application loop
    char choice[10];
    printf("Select menu: ");
    fgets(choice, sizeof(choice), stdin);
}

pj_status_t thr_pjsip_init(void)
{
    pj_status_t res = -1;

    res = app_init();
    if (res != PJ_SUCCESS) 
        return res;

    res = pjsua_start();
    if (res != PJ_SUCCESS)
    {
        pjsua_destroy();
        pjsua_perror(THIS_FILE, "Error starting pjsua", res);
    }

    return res;
}

pj_status_t thr_pjsip_loop(void)
{
    app_run();
}