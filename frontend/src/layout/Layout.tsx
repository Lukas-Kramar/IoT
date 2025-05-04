// import React from 'react';
import { Outlet } from 'react-router-dom';
import Menu from './header/Menu';
import { OrganisationsContextProvider } from '../customHooks/useOrganisationsContext';
import { memo } from 'react';

const Layout = () => {
    return (
        <OrganisationsContextProvider>
            <div>
                <header>
                    <Menu />
                </header>
                {/* Bootstrap Navbar default height  */}
                <main style={{ marginTop: '56px', paddingTop: '10px' }}>
                    <Outlet />
                </main>
            </div>
        </OrganisationsContextProvider>
    );
}

export default memo(Layout);